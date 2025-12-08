module heat;
#include "formloader.h"
import config;
import utility;
import generalStealing;

constexpr float MIN_HEAT_INCREASE = 2.0f;
constexpr float MAX_HEAT_INCREASE = 10.0f;
constexpr float HEAT_MAX_ITEM_VALUE = 1200.0f;
constexpr float MAX_HEAT_VALUE = 100.0f;
constexpr int MIN_ATTACK_CHANCE = 1;
constexpr int MAX_ATTACK_CHANCE = 50;

float HeatSystem::GetHeatValue()
{
	return FormLoader::Loader::GetSingleton()->player_heat_system_global->value;
}

void HeatSystem::SetHeatValue(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	formL->player_heat_system_global->value = a_value;
}

void HeatSystem::IncreaseHeat(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	REX::DEBUG("Heat global before increasing is: {}", GetHeatValue());
	formL->player_heat_system_global->value = std::clamp(formL->player_heat_system_global->value + a_value, 0.0f, MAX_HEAT_VALUE);
	REX::DEBUG("Heat global is: {}", GetHeatValue());
}

void HeatSystem::DecreaseHeat(float a_value)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	REX::DEBUG("Heat global before decreasing is: {}", GetHeatValue());
	formL->player_heat_system_global->value = std::clamp(formL->player_heat_system_global->value - a_value, 0.0f, MAX_HEAT_VALUE);
	REX::DEBUG("Heat global is: {}", GetHeatValue());	
}

void HeatSystem::ClearHeat()
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	formL->player_heat_system_global->value = 0;
}

bool HeatSystem::IsHeatAbove(float a_threshold)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	return formL->player_heat_system_global->value > a_threshold;
}

void HeatSystem::UpdateHeatValueExternal()
{
	float min_value = static_cast<float>(Config::Settings::GetSingleton()->fence_value_heat_threshold.GetValue());

	if (min_value >= HEAT_MAX_ITEM_VALUE) 
		min_value = HEAT_MAX_ITEM_VALUE - 1.0f;
	
	float value = static_cast<float>(HeatValues::item_value_fenced);
	if (value >= min_value) {		
		float heat_increase = MIN_HEAT_INCREASE;

		float scale = (MAX_HEAT_INCREASE - MIN_HEAT_INCREASE) / (HEAT_MAX_ITEM_VALUE - min_value);
		heat_increase += (value - min_value) * scale;

		if (heat_increase > MAX_HEAT_INCREASE)
			heat_increase = MAX_HEAT_INCREASE;

		HeatSystem::IncreaseHeat(heat_increase);
	}
}

//timer instance of heat system 

static UtilTimer g_heat_decrease_timer;
static UtilTimer g_wait_in_crime_timer;
static UtilTimer g_heat_increase_timer;
static UtilTimer g_bounty_decrease_timer;
static bool timer_was_run = false;

void AllCrimeTimersStop() {
	g_heat_increase_timer.Stop();
	g_wait_in_crime_timer.Stop();
	timer_was_run = false;
}

bool IsAnyCrimeTimerRunning() {
	return g_heat_increase_timer.IsRunning() || g_wait_in_crime_timer.IsRunning();
}

void StartIngameHourTimer() {
	if (!g_heat_decrease_timer.IsRunning()) {
		RE::Calendar* cal = RE::Calendar::GetSingleton();		
		g_heat_decrease_timer.Start(1800 / cal->GetTimescale());
		REX::DEBUG("started decay timer for {} seconds", g_heat_decrease_timer.GetExpectedRuntime());
	}
}

void HeatSystem::UpdatePassiveHeatDecay() {
	if (!IsAnyCrimeTimerRunning()) {
		if (!g_heat_decrease_timer.IsRunning()) {
			StartIngameHourTimer();
		} else if (g_heat_decrease_timer.Elapsed() >= g_heat_decrease_timer.GetExpectedRuntime()) {
			g_heat_decrease_timer.Reset();
			DecreaseHeat(Config::Settings::hourly_heat_decrease.GetValue());
			REX::DEBUG("Decrease heat over time");
		}
	} else {
		g_heat_decrease_timer.Stop();
	}
}

std::int32_t HeatSystem::ReCalculateDetectionLevel(std::int32_t a_level)
{
	if (a_level < 0) {
		a_level += 100;
		return a_level = std::clamp(a_level, 0, 100);
	} else
		return 100;
}

std::pair<RE::TESBoundObject*, std::int32_t> HeatSystem::GetRandomInventoryItem(const RE::TESObjectREFR::InventoryItemMap& inventory)
{
	if (inventory.empty())
		return { nullptr, 0 };

	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<std::pair<RE::TESBoundObject*, std::int32_t>> miscItems;

	// Collect all misc items first
	for (const auto& [object, data] : inventory) {
		if (object && object->GetFormType() == RE::FormType::Misc) {
			miscItems.emplace_back(object, data.first);
		}
	}

	// If none found, return null
	if (miscItems.empty())
		return { nullptr, 0 };

	// Pick a random one
	std::uniform_int_distribution<size_t> dist(0, miscItems.size() - 1);
	auto& selected = miscItems[dist(gen)];

	return selected;
}

float HeatSystem::GetRandomPickpocketChance(float pickpocketSkill, float infamy, float reputation)
{
	constexpr float maxChance = 70.0f;

	float baseChance = (pickpocketSkill / 100.0f) * maxChance;
	float heatPenalty = (infamy / 100.0f) * (maxChance * 0.5f);
	float repBonus = (reputation > 250.0f) 
		? ((reputation - 250.0f) / 250.0f) * (maxChance * 0.2f)
		: 0.0f;

	float finalChance = baseChance - heatPenalty + repBonus;

	// Clamp to [0, maxChance]
	finalChance = std::clamp(finalChance, 0.0f, maxChance);
	return finalChance;
}

void HeatSystem::AddPickpocketExperience()
{
	const auto& player = RE::PlayerCharacter::GetSingleton();

	float HandToHandLevel = player->GetActorValue(RE::ActorValue::kPickpocket);

	float baseXP = (0.15f * HandToHandLevel) + 3.0f;

	player->AddSkillExperience(RE::ActorValue::kPickpocket, baseXP);	
}

// used for pickpocket while bump, infamy reduction, infamy conseqences
void HeatSystem::PlayerUpdateLoop(RE::PlayerCharacter* a_player, float a_delta)
{
	const auto& formL = FormLoader::Loader::GetSingleton();
	if (Utility::IsEffectActive(a_player, formL->sleight_of_hand_effect)) {
		if (!a_player->IsRunning() && !a_player->IsAttacking() && !a_player->IsInCombat() && !a_player->IsBlocking() && a_player->GetWeaponState() != RE::WEAPON_STATE::kDrawn) {
			RE::Actor* close_actor = Utility::GetClosestNonTeammate(a_player, 60.f);
			if (close_actor && !close_actor->IsGuard() && !a_player->GetCurrentScene() && a_player->IsInFaction(close_actor->GetCrimeFaction()) && !close_actor->IsChild() && close_actor->GetRace()->HasKeywordString("ActorTypeNPC")) {
				RE::TESObjectREFR::InventoryItemMap inv_map = close_actor->GetInventory();
				auto random_item = GetRandomInventoryItem(inv_map);
				if (random_item.first && random_item.second > 0 && !actor_pickpocketed.contains(close_actor)) {
					actor_pickpocketed.insert(close_actor);
					REX::DEBUG("attempting to pickpocket item: {} with amount: {}", random_item.first->GetName(), random_item.second);
					if (Randomiser::GetRandomFloat(0.0f, 100.0f) < GetRandomPickpocketChance(a_player->GetActorValue(RE::ActorValue::kPickpocket), GetHeatValue(), Stealing::NightThief::GetNightReputation())) {
						REX::DEBUG("pickpocket success");					
						close_actor->RemoveItem(random_item.first, random_item.second, RE::ITEM_REMOVE_REASON::kSteal, nullptr, a_player);
						AddPickpocketExperience();
						RE::SendHUDMessage::ShowHUDMessage(std::format("You successfully pickpocketed {} from {}.",random_item.first->GetName(),close_actor->GetName()).c_str());
					}
					else {
						REX::DEBUG("pickpocket failed");
						// if you fail, you get a chance to be attacked by the victim
						int random_int = Randomiser::GetRandomInt(0,100);
						if (random_int >= 25) {
							Crimes::SendAssaultAlarm(close_actor, a_player, 0 , 0);
							Crimes::unk_Assault(a_player->currentProcess, a_player, close_actor);
						}
					}
				}
			}
		}	
	}

	if (a_player->HasPerk(formL->lower_bounty_perk)) {
		if (!g_bounty_decrease_timer.IsRunning()) {
			g_bounty_decrease_timer.Start(120);
		}
		if (g_wait_in_crime_timer.IsRunning() && g_wait_in_crime_timer.Elapsed() >= g_wait_in_crime_timer.GetExpectedRuntime()) {
			Stealing::CrimeTracker::LowerAllBounties(5.0f);
			g_bounty_decrease_timer.Reset();
		}
	}
	else {
		if (g_bounty_decrease_timer.IsRunning())
			g_bounty_decrease_timer.Stop();
	}
	

	if (a_player->GetParentCell() != UtilStates::curr_crime_cell && UtilStates::IsInCrimeScene() || !a_player->GetParentCell()) {
		UtilStates::SetInCrimeScene(false);
		AllCrimeTimersStop();
		REX::DEBUG("left crime scene. stop all waiting timers");
	}
	if (UtilStates::IsInCrimeScene() && !timer_was_run) {
		if (!g_wait_in_crime_timer.IsRunning()) {
			g_wait_in_crime_timer.Start(120);
			REX::DEBUG("crime scene timer is running");
			timer_was_run = true;
		}			
	}
	if (g_wait_in_crime_timer.IsRunning() && g_wait_in_crime_timer.Elapsed() >= g_wait_in_crime_timer.GetExpectedRuntime()) {
		g_wait_in_crime_timer.Stop();
		REX::DEBUG("wait time's over, start to gain infamy");
		if (!g_heat_increase_timer.IsRunning()) {
			g_heat_increase_timer.Start(30);
		}
		IncreaseHeat(5.0f);
	}
	if (g_heat_increase_timer.IsRunning() && g_heat_increase_timer.Elapsed() >= g_heat_increase_timer.GetExpectedRuntime()) {
		if (UtilStates::IsInCrimeScene()) {
			g_heat_increase_timer.Reset();
			IncreaseHeat(4.0f);
		}
		else
			g_heat_increase_timer.Stop();
	}
	UpdatePassiveHeatDecay();
	return _Hook9(a_player, a_delta);
}

void HeatSystem::OnGoToPrison(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_removeInventory, bool a_realJail)
{
	if (a_realJail) {
		REX::DEBUG("went to jail");
		ClearHeat();
	}
	else {
		DecreaseHeat(3.0f);
	}
	_Hook8(a_this, a_faction, a_removeInventory, a_realJail);
}

void HeatSystem::PayCrime(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_goToJail, bool a_removeStolenItems)
{
	auto fine_now = a_this->GetCrimeGoldValue(a_faction);
	_Hook7(a_this, a_faction, a_goToJail, a_removeStolenItems);
	auto& total_crimes = a_this->crimeGoldMap;

	if (total_crimes.empty()) {
		REX::DEBUG("no more crimes, clear heat");
		ClearHeat();
	}
	if (fine_now >= a_faction->crimeData.crimevalues.murderCrimeGold) {
		// Don't reduce heat for murder bounties
		return;
	}
	REX::DEBUG("fine now is {}", fine_now);
	if (fine_now <= 100) {
		DecreaseHeat(2.0f);
	} else if (fine_now <= 150) {
		DecreaseHeat(4.0f);
	} else if (fine_now <= 300) {
		DecreaseHeat(3.0f);
	} else {
		DecreaseHeat(1.0f);
	}
}

void HeatSystem::DoCalculateDetection(RE::Actor* a_this, RE::Actor* target, std::int32_t& score, bool& spotted, bool& hasLOS, std::int32_t& reason, RE::NiPoint3& lastPos,  std::int32_t& soundLvl, float& unk8, float& unk9)
{
	_Hook10(a_this, target, score, spotted, hasLOS, reason, lastPos, soundLvl, unk8, unk9);	
	if (Utility::IsGuardNearby(target, 512) && target->IsSneaking()) {
		RE::SendHUDMessage::ShowHUDMessage("Your presence is noticed...");
		if (spotted) {
			auto infamy = GetHeatValue();
			if (infamy >= 90.0f && a_this->IsGuard()) {
				int random_int = Randomiser::GetRandomInt(MIN_ATTACK_CHANCE, MAX_ATTACK_CHANCE);
				if (random_int == 1) {
					Crimes::unk_Assault(a_this->currentProcess, a_this, target);
				}			
			}
		}
		return;
	}	
}

void ReputationPerkHandler::RegisterCellEvent()
{
	if (auto player = RE::PlayerCharacter::GetSingleton()) {
		player->AddEventSink<RE::BGSActorCellEvent>(ReputationPerkHandler::GetSingleton());
	}
	REX::DEBUG("Registered for {}", typeid(ReputationPerkHandler).name());
}

RE::BSEventNotifyControl ReputationPerkHandler::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*)
{
	if (!a_event)
		return RE::BSEventNotifyControl::kContinue;

	

	if (a_event->flags.get() == RE::BGSActorCellEvent::CellFlag::kEnter) {

		if (!HeatSystem::actor_pickpocketed.empty()) {
			HeatSystem::actor_pickpocketed.clear();
		}
		const auto& player = RE::PlayerCharacter::GetSingleton();
		const auto& forms = FormLoader::Loader::GetSingleton();
		const auto cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(a_event->cellID);
		bool _foundStuff = false;
		if (!cell->IsInteriorCell() || forms->exception_cell_formlist->HasForm(cell->GetLocation())) {
			return RE::BSEventNotifyControl::kContinue;
		}
		if (CellIsDangerousLocation(cell, danger_loc)) {
			return RE::BSEventNotifyControl::kContinue;
		}
		
		if (!player->HasPerk(forms->gold_rush_perk)) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto& valuables = GetValuablesInCell(cell);

		if (!valuables.empty()) {
			_foundStuff = true;
			const auto& settings = Config::Settings::GetSingleton();
			for(const auto& item : valuables)
			{			
				item->ApplyEffectShader(forms->glow_shader, settings->gold_rush_shader_duration.GetValue());
				REX::DEBUG("Found valuable item: {} in cell: {}", item->GetName(), cell->GetName());

			}		
			const char* sound = nullptr;
			if (settings->enable_gold_rush_sound.GetValue()) {
				sound = "ST_GoldRushCoinSoundSD";
			}
			std::jthread{ [=] {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				SKSE::GetTaskInterface()->AddTask([=] {					
					RE::SendHUDMessage::ShowHUDMessage(settings->screen_notif_text.GetValue().c_str(), sound);
					});
				} }.detach();	
		}		
	}
	return RE::BSEventNotifyControl::kContinue;
}

bool ReputationPerkHandler::ReferenceIsInventoryItem(RE::TESForm* a_ref)
{
	if (a_ref) {
		switch (a_ref->GetFormType()) {
		case RE::FormType::Armor:
		case RE::FormType::Weapon:
		case RE::FormType::Misc:
		case RE::FormType::AlchemyItem:
		case RE::FormType::Ammo:
		case RE::FormType::Book:
		case RE::FormType::Flora:
		case RE::FormType::Ingredient:
			return true;
		default :
			return false;
		}
	}
	return false;
	
}

bool ReputationPerkHandler::CellIsDangerousLocation(RE::TESObjectCELL* a_cell, const std::unordered_set<std::string_view>& keywords)
{
	if (!a_cell || !a_cell->GetLocation())
		return true;

	for (const auto& keyword : keywords) {
		if (a_cell->GetLocation()->HasKeywordString(keyword)) {
			return true;
		}
	}

	return false;
}

bool ReputationPerkHandler::AreValuablesInCell(RE::TESObjectCELL* a_cell)
{
	bool found_item = false;
	static const auto& settings = Config::Settings::GetSingleton();
	a_cell->ForEachReference([this, &found_item](RE::TESObjectREFR* ref) {	
		auto base = ref->GetBaseObject();
		if (!base)
			return RE::BSContainer::ForEachResult::kContinue;
		auto extra = ref->extraList.GetByType<RE::ExtraEnchantment>();

		if (ReferenceIsInventoryItem(base)) {
			if (base->GetGoldValue() > settings->reputation_min_item_value.GetValue() || extra) {
				found_item = true;
				return RE::BSContainer::ForEachResult::kStop;
			}
		}					
		if (base->GetFormType() == RE::FormType::Container) {
			RE::TESContainer* const &container = ref->GetContainer();
			container->ForEachContainerObject([&](RE::ContainerObject const &entry) -> RE::BSContainer::ForEachResult {
				auto object = entry.obj;
				if (object->Is(RE::FormType::LeveledItem)) {
					auto list = object->As<RE::TESLeveledList>();
					RE::BSScrapArray<RE::CALCED_OBJECT> calcedObjects{};
					list->CalculateCurrentFormList(RE::PlayerCharacter::GetSingleton()->GetLevel(), static_cast<int16_t>(entry.count), calcedObjects, 0, true);
					for (auto& calcObj : calcedObjects) {
						if (calcObj.form->GetGoldValue() > settings->reputation_min_item_value.GetValue()) {
							found_item = true;
							return RE::BSContainer::ForEachResult::kStop;
						}
					}
				}								
				else{
					if (object && object->GetGoldValue()  > settings->reputation_min_item_value.GetValue()) {								
					found_item = true;
					return RE::BSContainer::ForEachResult::kStop;
					}
				}				
				return RE::BSContainer::ForEachResult::kContinue;
				});
		}
		return RE::BSContainer::ForEachResult::kContinue;
		});
	return found_item;
}

std::vector<RE::TESObjectREFR*> ReputationPerkHandler::GetValuablesInCell(RE::TESObjectCELL* a_cell)
{
#ifdef ENABLE_DEBUGGING
	//timer to measure performance. Debug only
	Timer po3Timer;
	po3Timer.start();	
#endif
	std::vector<RE::TESObjectREFR*> valuables;
	bool found_item = false;
	static const auto& settings = Config::Settings::GetSingleton();
	a_cell->ForEachReference([this, &valuables](RE::TESObjectREFR* ref) {	
		auto base = ref->GetBaseObject();
		if (!base)
			return RE::BSContainer::ForEachResult::kContinue;
		auto extra = ref->extraList.GetByType<RE::ExtraEnchantment>();

		if (ReferenceIsInventoryItem(base)) {
			if (base->GetGoldValue() > settings->reputation_min_item_value.GetValue() || extra) {
				valuables.emplace_back(ref);
				return RE::BSContainer::ForEachResult::kContinue;
			}
		}					
		if (base->GetFormType() == RE::FormType::Container) {
			RE::TESContainer* const &container = ref->GetContainer();
			container->ForEachContainerObject([&](RE::ContainerObject const &entry) -> RE::BSContainer::ForEachResult {
				auto object = entry.obj;
				if (object->Is(RE::FormType::LeveledItem)) {
					auto list = object->As<RE::TESLeveledList>();
					RE::BSScrapArray<RE::CALCED_OBJECT> calcedObjects{};
					list->CalculateCurrentFormList(RE::PlayerCharacter::GetSingleton()->GetLevel(), static_cast<int16_t>(entry.count), calcedObjects, 0, true);
					for (auto& calcObj : calcedObjects) {
						if (calcObj.form->GetGoldValue() > settings->reputation_min_item_value.GetValue()) {
							valuables.emplace_back(ref);
							return RE::BSContainer::ForEachResult::kStop;
						}
					}
				}								
				else{
					if (object && object->GetGoldValue()  > settings->reputation_min_item_value.GetValue()) {								
						valuables.emplace_back(ref);
						return RE::BSContainer::ForEachResult::kContinue;
					}
				}				
				return RE::BSContainer::ForEachResult::kContinue;
				});
		}
		return RE::BSContainer::ForEachResult::kContinue;
		});
#ifdef ENABLE_DEBUGGING
	po3Timer.end();
	REX::DEBUG("timer ran for {}", po3Timer.duration());
#endif
	return valuables;
}
