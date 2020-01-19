#pragma once
#include "pch.h"
#include "PointDisplay.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;


// the class definition for the core "framework" of our app
ref class App sealed : public IFrameworkView {
public:
    virtual void Initialize(CoreApplicationView^ AppView);
    virtual void SetWindow(CoreWindow^ Window);
    virtual void Load(String^ EntryPoint);
    virtual void Run();
    virtual void Uninitialize();

    // event handlers
    void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args);
    void PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args);
    void KeyDown(CoreWindow^ Window, KeyEventArgs^ Args);
    void Closed(CoreWindow^ Window, CoreWindowEventArgs^ Args);

private:
    bool WindowClosed;
    PointDisplay display;
};

