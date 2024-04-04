class CallbackWrapper
{
public:
    template<typename Function, typename... Args>
    CallbackWrapper(Function&& CallbackFunction, Args&&... Arguments)
        : Callback(std::bind(std::forward<Function>(CallbackFunction), std::forward<Args>(Arguments)...))
    {

    }

    ~CallbackWrapper()
    {

    }

    virtual void Call()     // Call the callback function, does not destroy the class
    {
        Callback();
    }

protected:
    std::function<void()> Callback;
};


void callbackFunction(int x, int y) {
    std::cout << "Callback called with arguments: " << x << " and " << y << std::endl;
}