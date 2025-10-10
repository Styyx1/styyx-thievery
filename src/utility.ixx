export module utility;

export struct UtilTimer {
    void Start(float expectedRuntime);
    void Stop();
    void Reset();
    [[nodiscard]] float Elapsed() const;
    [[nodiscard]] bool IsRunning() const;
    [[nodiscard]] float GetExpectedRuntime() const;

private:
    std::chrono::steady_clock::time_point startTime{};
    bool _isRunning{ false };
    float _expectedRuntime{ 0.0f };
};

export struct Utility {
    static void HideMenu(RE::BSFixedString a_menuName)
    {
        if (const auto UIMsgQueue = RE::UIMessageQueue::GetSingleton(); UIMsgQueue) {
            UIMsgQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        }
    }
    static float CalculateTimerForSkill(RE::PlayerCharacter* player, RE::ActorValue a_av, float min_time, float max_time) {
        float av_level = player->GetActorValue(a_av);
        float av_time = std::clamp(min_time * (1.5f + av_level / 50.0f), min_time, max_time);
        return av_time;
    }
    static RE::Actor* GetClosestNonTeammate(RE::TESObjectREFR* a_ref, float a_radius)
    {
        RE::Actor* result = nullptr;
        if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
            if (processLists->numberHighActors == 0) {
                REX::DEBUG("no process list");
                return result;
            }
            const auto squaredRadius = a_radius * a_radius;
            const auto originPos     = a_ref->GetPosition();

            const auto get_actor_within_radius = [&](RE::Actor* a_actor) {
                if (a_actor && a_actor != a_ref && !a_actor->IsPlayerTeammate() && originPos.GetSquaredDistance(a_actor->GetPosition()) <= squaredRadius) {
                    result = a_actor;
                }
            };

            for (auto& actorHandle : processLists->highActorHandles) {
                const auto actor = actorHandle.get();
                get_actor_within_radius(actor.get());
            }
        }
        if(result)
            REX::DEBUG("closest actor is: {}", result->GetDisplayFullName());
       
        return result;
    }

    static bool IsEffectActive(RE::Actor* a_actor, RE::EffectSetting* a_effect)
    {
        if (!a_actor || !a_effect) {
            return false;
        }
        auto activeEffects = a_actor->GetActiveEffectList();
        RE::EffectSetting* setting       = nullptr;
        if (!activeEffects->empty()) {
            for (RE::ActiveEffect* effect : *activeEffects) {
                if (effect; !effect->flags.any(RE::ActiveEffect::Flag::kInactive)) {
                    setting = effect ? effect->GetBaseObject() : nullptr;
                    if (setting) {
                        if (setting == a_effect) {
                            return true;
                        }
                    }
                }
            }
        } 
        return false;
	}

    static bool IsGuardNearby(RE::TESObjectREFR* a_ref, float a_radius)
    {
        bool result = false;
        if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
            if (processLists->numberHighActors == 0) {
                REX::DEBUG("no process list");
                return result;
            }
            const auto squaredRadius = a_radius * a_radius;
            const auto originPos     = a_ref->GetPosition();

            const auto get_actor_within_radius = [&](RE::Actor* a_actor) {
                if (a_actor && a_actor != a_ref && a_actor->IsGuard() && originPos.GetSquaredDistance(a_actor->GetPosition()) <= squaredRadius) {
                    result = a_actor;}
                };

            for (auto& actorHandle : processLists->highActorHandles) {
                const auto actor = actorHandle.get();
                get_actor_within_radius(actor.get());
            }
        }
        REX::DEBUG("Guard is nearby: {}", result ? "true" : "false");
        return result;
    }

    static RE::Setting* get_gmst(const char* a_setting)
    {
        return RE::GameSettingCollection::GetSingleton()->GetSetting(a_setting);
    }

    static void set_gmstFloat(const char* a_setting, float new_value) {
        const auto& gmstColl = RE::GameSettingCollection::GetSingleton();
        auto setting = gmstColl->GetSetting(a_setting);
        if (static_cast<int>(setting->GetFloat()) != static_cast<int>(new_value)) {            
            REX::DEBUG("{}: {} -> {}", setting->GetName(), setting->GetFloat(), new_value);
            setting->data.f = new_value;
        }       
    }
};

export struct Randomiser
{
    static std::mt19937& GetRNG()
    {
        static std::mt19937 gen(std::random_device{}());
        return gen;
    }

    static int GetRandomInt(int a_min, int a_max)
    {
        std::uniform_int_distribution<int> distrib(a_min, a_max);
        return distrib(GetRNG());
    }

    static float GetRandomFloat(float a_min, float a_max)
    {
        std::uniform_real_distribution<float> distrib(a_min, a_max);
        return distrib(GetRNG());
    }

    static float GetRoundedFloat(float a_min, float a_max, int decimals = 2)
    {
        auto value = GetRandomFloat(a_min, a_max);
        float factor = std::pow(10.0f, static_cast<float>(decimals));
        return std::round(value * factor) / factor;
    }
};
static std::mutex state_mutex;
export struct UtilStates {
    static inline bool inCrimeScene = false;   

    static void SetInCrimeScene(bool enable) {
        std::lock_guard<std::mutex> lock(state_mutex);
        inCrimeScene = enable;
    }

    static bool IsInCrimeScene() {
        std::lock_guard<std::mutex> lock(state_mutex);
        return inCrimeScene;
    }

    static RE::TESObjectCELL* curr_crime_cell;
};

export struct Crimes 
{
    static uint64_t GetCrimeValue(RE::Actor* a_actor, RE::PackageNS::CRIME_TYPE a_crime, RE::TESFaction* a_faction, int32_t a_itemValue = 0) {
        using func_t = decltype(&GetCrimeValue);
        REL::Relocation<func_t> func{ REL::ID(37680) }; //SE 36672
        return func(a_actor, a_crime, a_faction, a_itemValue);
    }

    static void SendTrespassingAlarm(RE::Actor* a_this, RE::Actor* a_caughtBy, RE::TESNPC* a_caughtByBase, RE::PackageNS::CRIME_TYPE a_crimeType = RE::PackageNS::CRIME_TYPE::kTrespass) {
        using func_t = decltype(&SendTrespassingAlarm);
        REL::Relocation<func_t> func{ REL::ID(37427) }; // SE 36432
        return func(a_this, a_caughtBy, a_caughtByBase, a_crimeType);
    }

    static bool unk_Assault(RE::AIProcess* a_victimProcess, RE::Actor* a_victim, RE::Actor* a_aggressor, int64_t param_4 = 0, float param_5 = 0) {
        using func_t = decltype(&unk_Assault);
        REL::Relocation<func_t> func{ REL::ID(39359) }; // SE 38375
        return func(a_victimProcess, a_victim, a_aggressor, param_4, param_5);
    }

    static void SendAssaultAlarm(RE::Actor* a_this, RE::Actor* a_attacker, int32_t a3, float a4) {
        using func_t = decltype(&SendAssaultAlarm);
        REL::Relocation<func_t> func{ REL::ID(37424) }; //SE 36429
        return func(a_this, a_attacker, a3, a4);
    }

    static void SendStealAlarm(RE::Actor* a_this, RE::Actor * a_thief, RE::TESObjectREFR * a_stolenItemRef, RE::TESForm * a_stolenItemBase, int32_t a_stolenItemCount, int32_t a_itemWorth, RE::TESForm * a_owner, bool a8) {
        using func_t = decltype(&SendStealAlarm);
		REL::Relocation<func_t> func{ REL::ID(37422) }; // SE 36427
        return func(a_this, a_thief, a_stolenItemRef, a_stolenItemBase, a_stolenItemCount, a_itemWorth, a_owner, a8);
    }
};