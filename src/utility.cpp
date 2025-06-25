module utility;

void UtilTimer::Start(float expectedRuntime)
{
    startTime = std::chrono::steady_clock::now();
    _expectedRuntime = expectedRuntime;
    _isRunning = true;
}

void UtilTimer::Stop()
{
    _isRunning = false;
}

void UtilTimer::Reset()
{
    startTime = std::chrono::steady_clock::now();
}

float UtilTimer::Elapsed() const
{
    if (!_isRunning)
        return 0.0f;

    auto now = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    return dur / 1000.0f;
}

bool UtilTimer::IsRunning() const
{
    return _isRunning;
}

float UtilTimer::GetExpectedRuntime() const
{
    return _expectedRuntime;
}
RE::TESObjectCELL* UtilStates::curr_crime_cell = nullptr;