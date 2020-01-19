#include "pch.h"
#include "PointDisplay.h"
#include "App.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

ref class AppSource sealed : IFrameworkViewSource {
public:
    virtual IFrameworkView^ CreateView() {
        return ref new App();
    }
};

[MTAThread]

int main(Array<String^>^ args) {
    CoreApplication::Run(ref new AppSource());
    return 0;
}