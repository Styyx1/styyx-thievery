module utility;

void Timer::Start(float expectedRuntime)
{
    startTime = std::chrono::steady_clock::now();
    _expectedRuntime = expectedRuntime;
    _isRunning = true;
}

void Timer::Stop()
{
    _isRunning = false;
}

void Timer::Reset()
{
    startTime = std::chrono::steady_clock::now();
}

float Timer::Elapsed() const
{
    if (!_isRunning)
        return 0.0f;

    auto now = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    return dur / 1000.0f;
}

bool Timer::IsRunning() const
{
    return _isRunning;
}

float Timer::GetExpectedRuntime() const
{
    return _expectedRuntime;
}