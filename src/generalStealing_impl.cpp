
module generalStealing;
#include "formloader.h"
import config;


namespace Stealing {
	static std::mutex isResetting;

	void NightThief::DecreaseReputation() {

		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;
		if (reput)
			reput->value = std::clamp(std::round(reput->value * 0.8f), 0.0f, 500.0f);
	}

	void NightThief::IncreaseNightReputation(uint16_t amount)
	{
		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;
		if (reput)
			reput->value += amount;
	}

	uint32_t NightThief::GetNightReputation() {

		const auto& form = FormLoader::Loader::GetSingleton();
		auto& reput = form->night_reputation_global;

		return reput ? static_cast<uint32_t>(reput->value) : 0;
	}

	bool NightThief::IsNight() {
		auto calendar = RE::Calendar::GetSingleton();
		return calendar->GetHour() < 6.0 || calendar->GetHour() > 21.0;
	}

	void NightThief::PickupObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound)
	{
		_Hook2(a_this, a_object, a_count, a_arg3, a_playSound);
		auto ownership = a_object->extraList.GetByType<RE::ExtraOwnership>();
		if (ownership) {
			if (ownership->owner != a_this && a_object->GetBaseObject() && a_object->GetBaseObject()->GetGoldValue() >= Config::Settings::GetSingleton()->reputation_min_item_value.GetValue()) {
				if (IsNight()) {
					IncreaseNightReputation(a_count);
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
					IncreaseNightReputation(a_count);
				}
			}
		}
	}
	// function runs whenever you get a bounty. effectively when you're caught. 
	void NightThief::OnGettingCaught(RE::PlayerCharacter* a_this, RE::Actor* a_caughtBy, float a3)
	{
		DecreaseReputation(); // decrease your thief reputation when you are getting caught.
		_Hook3(a_this, a_caughtBy, a3);
	}

	void Stealing::NightThiefFencing::Call(Params & a_params)
	{
		if (a_params.argCount >= 1) {
			auto&& root = a_params.thisPtr;

			auto&& a_updateObj = a_params.args[0];

			RE::GFxValue res(RE::GFxValue::ValueType::kBoolean);
			root->Invoke("isViewingVendorItems", &res);

			using _Callback = float(*)(RE::Actor*, RE::InventoryEntryData* objDesc, uint16_t a_level, RE::GFxValue& a_updateObj, bool is_buying);

			auto&& formID = a_this->itemList->GetSelectedItem()->data.objDesc->GetObject()->formID;
			auto object = a_this->itemList->GetSelectedItem()->data.objDesc;
			
			auto TraderRefhandle = a_this->GetTargetRefHandle();
			auto trader = RE::TESObjectREFR::LookupByHandle(TraderRefhandle)->As<RE::Actor>();

			if (trader)
				REX::INFO("trader is {}", trader->GetDisplayFullName());

			if (object->object) {
				float mult = 1.0f;
				RE::GFxValue isBuying(RE::GFxValue::ValueType::kBoolean);
				a_updateObj.GetMember("is_buying", &isBuying);
				if (isBuying.IsBool())
					REX::INFO("isBuying is {}", isBuying.GetBool() ? "true" : "false");

				if (!object->IsOwnedBy(RE::PlayerCharacter::GetSingleton())) {
					REX::INFO("vendor {} is buying a stolen {}", trader->GetDisplayFullName(), object->object->GetName());
					mult = CalculateNightThiefPrice();
					IncreaseFencedItemCountAndAmount(1, object->GetValue());
				}
				else
					mult = 1.0f;

				RE::GFxValue value(RE::GFxValue::ValueType::kNumber);
				a_updateObj.GetMember("value", &value);
				value.SetNumber(value.GetNumber() * mult);
				a_updateObj.SetMember("value", value);
			}
		}
		oldFunc.Invoke("call", a_params.retVal, a_params.argsWithThisRef, a_params.argCount + 1);
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

    REX::INFO("Trader in PostCreate is: {}", trader->GetName());

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
		loader->total_amount_item_fenced->value += item_count;
		loader->total_value_item_fenced->value += item_value;
	}

}



