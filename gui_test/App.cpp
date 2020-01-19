#include "pch.h"
#include "App.h"
#include "PointDisplay.h"

void App::Initialize(CoreApplicationView^ AppView) {
    AppView->Activated += ref new TypedEventHandler
        <CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
    WindowClosed = false;
}

void App::SetWindow(CoreWindow^ Window) {
    //add ssome event habdlers
    Window->PointerPressed += ref new TypedEventHandler <CoreWindow^, PointerEventArgs^>(this, &App::PointerPressed);
    Window->KeyDown += ref new TypedEventHandler <CoreWindow^, KeyEventArgs^>(this, &App::KeyDown);
    Window->Closed += ref new TypedEventHandler <CoreWindow^, CoreWindowEventArgs^>(this, &App::Closed);
}

void App::Load(String^ EntryPoint) {}

void App::Run() {
    CoreWindow^ Window = CoreWindow::GetForCurrentThread();
    display.Initialaze();

    while (!WindowClosed) {
        Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        display.Update();
        display.Render();
    }
}

void App::Uninitialize() {}

//event handlers
void App::OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {
    CoreWindow^ Window = CoreWindow::GetForCurrentThread();
    Window->Activate();
}

void App::PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args) {
    MessageDialog dialog("Thank you for the attention!", "Attention!");
    dialog.ShowAsync();
}

void App::KeyDown(CoreWindow^ Window, KeyEventArgs^ Args) {
    MessageDialog dialog("Man down!", "Aaw shit");
    dialog.ShowAsync();
}

void App::Closed(CoreWindow^ Window, CoreWindowEventArgs^ Args) {
    WindowClosed = true;
}