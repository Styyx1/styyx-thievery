module generalStealing;

#include "formloader.h"
#undef min
#undef max
#undef GetForm
#undef GetObject
import utility;
import config;
import heat;

namespace Stealing {
	static std::mutex isResetting;

	void NightThief::DecreaseReputation(float a_pctModifier) {

		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;
		float modifier = 1.0f - std::clamp(a_pctModifier, 0.0f, 1.0f);
		if (reput)
			reput->value = std::clamp(std::round(reput->value * modifier), 0.0f, 500.0f);
	}

	void NightThief::IncreaseNightReputation(uint16_t amount)
	{
		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;
		if (reput)
			reput->value += amount;
	}

	uint16_t NightThief::CalculateReputationGain(uint32_t itemValue)
	{
		if (itemValue < static_cast<uint32_t>(Config::Settings::GetSingleton()->reputation_min_item_value.GetValue()))
			return 0;

		float gain = 1.0f + ((itemValue - 50.0f) / 550.0f) * 9.0f;
		return static_cast<uint16_t>(std::clamp(std::round(gain), 1.0f, 10.0f));
	}

	uint32_t NightThief::GetNightReputation() {

		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;

		return reput ? static_cast<uint32_t>(reput->value) : 0;
	}

	void NightThief::CalculateReputationLootChance()
	{
		const auto& forms = FormLoader::Loader::GetSingleton();
		float pct_rep = forms->night_reputation_global->value / 500.0f;
		float loot_mod = 100.0f * (1.0f - pct_rep);
		forms->fence_rep_special_item_global->value = loot_mod;
	}

	bool NightThief::IsNight() {
		auto calendar = RE::Calendar::GetSingleton();
		return calendar->GetHour() < 6.0 || calendar->GetHour() > 21.0;
	}

	float NightThiefFencing::GetBarterPriceFactor(RE::Actor* player)
	{
		float fBarterMax = Utility::get_gmst("fBarterMax")->GetFloat();
		float fBarterMin = Utility::get_gmst("fBarterMin")->GetFloat();
		const float speechSkill = std::clamp(player->GetActorValue(RE::ActorValue::kSpeech), 0.0f, 200.0f);

		return fBarterMax - (fBarterMax - fBarterMin) * (speechSkill / 100.0f);
	}

	void NightThief::PickupObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound)
	{
		_Hook2(a_this, a_object, a_count, a_arg3, a_playSound);
		auto ownership = a_object->extraList.GetByType<RE::ExtraOwnership>();
		if (ownership) {
			if (ownership->owner != a_this && a_object->GetBaseObject() && a_object->GetBaseObject()->GetGoldValue() >= Config::Settings::GetSingleton()->reputation_min_item_value.GetValue()) {
				if (IsNight()) {					
					IncreaseNightReputation(CalculateReputationGain(a_object->GetGoldValue() * a_count));
					CalculateReputationLootChance();
					if (!UtilStates::IsInCrimeScene()) {
						UtilStates::SetInCrimeScene(true);
						UtilStates::curr_crime_cell = a_this->GetParentCell();
					}
				}			
			}
		}
	}

	void NightThief::OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr)
	{
		_Hook1(a_this, a_object, a_extraList, a_count, a_fromRefr);
		if (a_extraList && a_extraList->GetOwner() != a_this) {
			if (a_object->GetGoldValue() >= Config::Settings::GetSingleton()->reputation_min_item_value.GetValue()) {
				if (IsNight()) {
					IncreaseNightReputation(CalculateReputationGain(a_object->GetGoldValue() * a_count));
					CalculateReputationLootChance();
					if (!UtilStates::IsInCrimeScene()) {
						UtilStates::SetInCrimeScene(true);
						UtilStates::curr_crime_cell = a_this->GetParentCell();
					}
				}
			}
		}
	}

	RE::ExtraDataList* NightThief::GetExtraData(RE::InventoryEntryData* a_inventoryEntry, int a_soldItems, bool is_equipped)
	{
		auto extra = _Hook6(a_inventoryEntry, a_soldItems, is_equipped);
		if (a_inventoryEntry) {
			auto obj = a_inventoryEntry->GetObject();
			REX::DEBUG("object is {}", obj->GetName());
			REX::DEBUG("is_equipped for {} is {}", obj->GetName(), is_equipped ? "true" : "false");
		}
		if (a_soldItems) {
			REX::DEBUG("number is: {}",a_soldItems);
		}

		if (extra) {
			if (extra->GetOwner() != RE::PlayerCharacter::GetSingleton()) {

				REX::DEBUG("selling stolen item with a worth of: {}", HeatValues::item_value_fenced);
				NightThiefFencing::IncreaseFencedItemCountAndAmount(1, HeatValues::item_value_fenced);
				REX::DEBUG("Fence amounts: item value from item card: {}", HeatValues::item_value_fenced);
				HeatSystem::UpdateHeatValueExternal();				
			}
		}
		return extra;
	}

	void NightThief::ModCrimeGoldValueHook(RE::Actor* a_this, RE::TESFaction* a_faction, bool a_violent, int32_t a_amount)
	{
		auto chance_bounty = Randomiser::GetRandomInt(0, 100);
		if (a_this->HasPerk(FormLoader::Loader::GetSingleton()->no_bounty_perk) && !a_violent) {
			if (chance_bounty <= 10) {
				REX::DEBUG("Player has the no bounty perk, chance to not get a bounty: {}", chance_bounty);
				return; // 10% chance to not get a bounty if player has the no bounty perk
			}
		}

		_Hook3(a_this, a_faction, a_violent, a_amount);
		REX::DEBUG("info dump: actor is: {}, faction is: {}, violent crime bool is: {}, amount is: {}", a_this->GetName(), a_faction->GetName(), a_violent ? "true" : "false", a_amount);
		if (a_amount > 0 && !a_violent) {
			DecreaseReputation(0.2f); // decrease your thief reputation when you are getting caught.
			CalculateReputationLootChance();
			HeatSystem::IncreaseHeat(Randomiser::GetRoundedFloat(2.0f, 10.0f));
		}
		else if (a_amount > 0 && a_violent) {
			DecreaseReputation(0.4f);
			CalculateReputationLootChance();
			HeatSystem::IncreaseHeat(Randomiser::GetRoundedFloat(10.0f, 20.0f));
		}
	}

	int32_t NightThiefFencing::GetTotalItemCountFenced()
	{
		const auto& forms = FormLoader::Loader::GetSingleton();
		return static_cast<int32_t>(forms->total_amount_item_fenced->value);
	}

	int32_t NightThiefFencing::GetTotalItemValueFenced()
	{
		const auto& forms = FormLoader::Loader::GetSingleton();
		return static_cast<int32_t>(forms->total_value_item_fenced->value);
	}

	void Stealing::NightThiefFencing::Call(Params & a_params)
	{
		if (a_params.argCount >= 1) {
			auto&& root = a_params.thisPtr;

			auto&& a_updateObj = a_params.args[0];

			RE::GFxValue res(RE::GFxValue::ValueType::kBoolean);
			root->Invoke("isViewingVendorItems", &res);

			using _Callback = float(*)(RE::Actor*, RE::InventoryEntryData* objDesc, uint16_t a_level, RE::GFxValue& a_updateObj, bool is_buying);

			auto object = a_this->itemList->GetSelectedItem()->data.objDesc;
			
			auto TraderRefhandle = a_this->GetTargetRefHandle();
			auto trader = RE::TESObjectREFR::LookupByHandle(TraderRefhandle)->As<RE::Actor>();

			if (trader)
				REX::DEBUG("trader is {}", trader->GetDisplayFullName());

			if (object->object) {
				float mult = 1.0f;
				if (!object->IsOwnedBy(RE::PlayerCharacter::GetSingleton())) {
					mult = CalculateNightThiefPrice();
					if(RE::PlayerCharacter::GetSingleton()->HasPerk(FormLoader::Loader::GetSingleton()->fence_price_increase_perk)) {
						mult *= 1.2f; // increase price by 20% if player has the fence price increase perk
					}
					RE::GFxValue value(RE::GFxValue::ValueType::kNumber);
					a_updateObj.GetMember("value", &value);
					value.SetNumber(value.GetNumber() * mult);
					a_updateObj.SetMember("value", value);
					// Calculate and set fenced item value for tracking, needs rounding to nearest int
					HeatValues::item_value_fenced = std::lround(value.GetNumber() / static_cast<double>(GetBarterPriceFactor(RE::PlayerCharacter::GetSingleton())));				
				}				
			}
		}
		oldFunc.Invoke("call", a_params.retVal, a_params.argsWithThisRef, static_cast<RE::UPInt>(a_params.argCount) + 1);
	}

	void NightThiefFencing::PostCreate(RE::BarterMenu* a_this)
	{
		_Hook4(a_this);

		auto traderRefHandle = a_this->GetTargetRefHandle();
		auto trader = RE::TESObjectREFR::LookupByHandle(traderRefHandle)->As<RE::Actor>();

		if (!trader) {
			REX::CRITICAL("[PostCreate] Couldn't find the trader of this trade attempt!");
			return;
		}

		REX::DEBUG("Trader in PostCreate is: {}", trader->GetName());

		auto& barter = *a_this;
		auto& root = barter.root;

		RE::GFxValue oldf;
		root.GetMember("UpdateItemCardInfo", &oldf);

		RE::GFxValue newf;
		auto impl = RE::make_gptr<NightThiefFencing>(std::move(oldf), a_this);
		barter.uiMovie->CreateFunction(&newf, impl.get());

		root.SetMember("UpdateItemCardInfo", newf);
	}

	float NightThiefFencing::CalculateNightThiefPrice()
	{
		uint32_t nightThiefLevel = static_cast<uint32_t>(FormLoader::Loader::GetSingleton()->night_reputation_global->value);
		nightThiefLevel = std::clamp(nightThiefLevel, 0u, 500u);
		return 1.0f + std::sqrt(nightThiefLevel / 500.0f) * (2.5f - 1.0f);
	}

	void NightThiefFencing::IncreaseFencedItemCountAndAmount(uint32_t item_count, uint32_t item_value)
	{
		const auto& loader = FormLoader::Loader::GetSingleton();
		REX::DEBUG("total before: {}", loader->total_value_item_fenced->value);
		loader->total_amount_item_fenced->value += item_count;		
		loader->total_value_item_fenced->value += item_value;
	}

	void CrimeTracker::LowerAllBounties(float a_amount)
	{
		if (a_amount <= 0.0f) {
			REX::ERROR("Invalid amount for LowerAllBounties");
			return;
		}
		const auto& player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return;
		}
		for (auto& [faction, bounty] : player->crimeGoldMap) {
			bounty.nonViolentCur = std::max(0.0f, bounty.nonViolentCur - a_amount);
			REX::DEBUG("Lowered bounty for faction {} by {}. New value: {}", faction->GetName(), a_amount, bounty.nonViolentCur);
		}
		REX::DEBUG("All bounties lowered by {}", a_amount);
	}

	void CrimeTracker::LowerBounty(RE::PlayerCharacter* a_player, RE::TESFaction* a_faction, float a_amount)
	{
		if (!a_player || !a_faction || a_amount <= 0.0f) {
			REX::ERROR("Invalid parameters for LowerBounty");
			return;
		}
		auto& tracked_bounties = a_player->crimeGoldMap;
		auto it = tracked_bounties.find(a_faction);
		if (it != tracked_bounties.end()) {
			it->second.nonViolentCur = std::max(0.0f, it->second.nonViolentCur - a_amount);
			REX::DEBUG("Lowered bounty for faction {} by {}. New value: {}", a_faction->GetName(), a_amount, it->second.nonViolentCur);
		} else {
			REX::ERROR("Faction not found in tracked bounties");
		}
	}
}