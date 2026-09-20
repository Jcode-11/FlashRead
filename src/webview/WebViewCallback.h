#pragma once

#ifdef _WIN32
#include <windows.h>
#include <wrl/client.h>
#include <atomic>
#include <functional>
#include <utility>

namespace FlashRead::WebView {

template <typename IHandler, typename... Args>
class CallbackImpl : public IHandler {
public:
    using FuncType = std::function<HRESULT(Args...)>;
    explicit CallbackImpl(FuncType func) : m_func(std::move(func)), m_ref(1) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override {
        if (!ppvObject) return E_POINTER;
        *ppvObject = static_cast<IHandler *>(this);
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return ++m_ref;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ref = --m_ref;
        if (ref == 0) delete this;
        return ref;
    }

    HRESULT STDMETHODCALLTYPE Invoke(Args... args) override {
        if (m_func) {
            return m_func(args...);
        }
        return S_OK;
    }

private:
    FuncType m_func;
    std::atomic<ULONG> m_ref;
};

template <typename IHandler, typename... Args, typename TFunc>
Microsoft::WRL::ComPtr<IHandler> MakeHandler(TFunc &&func) {
    return Microsoft::WRL::ComPtr<IHandler>(new CallbackImpl<IHandler, Args...>(std::forward<TFunc>(func)));
}

} // namespace FlashRead::WebView

#endif
