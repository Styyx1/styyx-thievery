export module utility;

export struct Timer {
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

export struct Crimes 
{
    static uint64_t GetCrimeValue(RE::Actor* a_actor, RE::PackageNS::CRIME_TYPE a_crime, RE::TESFaction* a_faction, int32_t a_itemValue = 0) {
        using func_t = decltype(&GetCrimeValue);
        REL::Relocation<func_t> func{ REL::ID(37680) };
        return func(a_actor, a_crime, a_faction, a_itemValue);
    }

    static void SendTrespassingAlarm(RE::Actor* a_this, RE::Actor* a_caughtBy, RE::TESNPC* a_caughtByBase, RE::PackageNS::CRIME_TYPE a_crimeType) {
        using func_t = decltype(&SendTrespassingAlarm);
        REL::Relocation<func_t> func{ REL::ID(37427) };
        return func(a_this, a_caughtBy, a_caughtByBase, a_crimeType);
    }

    static bool unk_Assault(RE::AIProcess* a_victimProcess, RE::Actor* a_victim, RE::Actor* a_aggressor, int64_t param_4 = 0, float param_5 = 0) {
        using func_t = decltype(&unk_Assault);
        REL::Relocation<func_t> func{ REL::ID(39359) };
        return func(a_victimProcess, a_victim, a_aggressor, param_4, param_5);
    }
};