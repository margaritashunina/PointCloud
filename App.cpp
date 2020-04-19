#include "pch.h"
#include "App.h"

void App::Initialize(CoreApplicationView^ AppView) {
    AppView->Activated += ref new TypedEventHandler
        <CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
    WindowClosed = false;
    PointerLocked = false;
    pointerPosition.x = 0;
    pointerPosition.y = 0;

    dt = ds = alph = bet = 0;
}

void App::SetWindow(CoreWindow^ Window) {
    //add ssome event habdlers
    Window->PointerMoved += ref new TypedEventHandler <CoreWindow^, PointerEventArgs^>(this, &App::PointerMoved);
    Window->PointerPressed += ref new TypedEventHandler <CoreWindow^, PointerEventArgs^>(this, &App::PointerPressed);
    Window->PointerReleased += ref new TypedEventHandler <CoreWindow^, PointerEventArgs^>(this, &App::PointerReleased);
    Window->KeyDown += ref new TypedEventHandler <CoreWindow^, KeyEventArgs^>(this, &App::KeyDown);
    Window->Closed += ref new TypedEventHandler <CoreWindow^, CoreWindowEventArgs^>(this, &App::Closed);
}

void App::Load(String^ EntryPoint) {}

void App::Run() {
    CoreWindow^ Window = CoreWindow::GetForCurrentThread();
    display.Initialaze();

    float timeFrame = (float)CLOCKS_PER_SEC / 30;
    //float updateColorFrame = (float)CLOCKS_PER_SEC / 240;
    int start = clock();
    //int startC = clock();
    while (!WindowClosed) {
        Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        
        int end = clock();

        /*
        if ((end - startC) > updateColorFrame) {
            display.UpdateColors();
            startC = clock();
        }
        */
        if ((end - start) > timeFrame) {
            display.Update(dt, ds, alph, bet);
            dt = ds = alph = bet = 0;
            display.Render();
            start = clock();
        }
    }
}

void App::Uninitialize() {}

//event handlers
void App::OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {
    CoreWindow^ Window = CoreWindow::GetForCurrentThread();
    Window->Activate();
}

void App::PointerMoved(CoreWindow^ Window, PointerEventArgs^ Args) {
    if (!PointerLocked) {
        return;
    }
    XMFLOAT2 newPos;
    newPos.x = Args->CurrentPoint->Position.X;
    newPos.y = Args->CurrentPoint->Position.Y;

    alph = (pointerPosition.x - newPos.x) * 90. / 1920.;
    bet = (pointerPosition.y - newPos.y) * 90. / 1080.;
    
    if (alph < 0) {
        alph += 360.;
    }
    //dx *= (float)5e-4 * -1.;
    //dy *= (float)5e-4;

    pointerPosition = newPos;
}

void App::PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args) {
    PointerLocked = true;
    pointerPosition.x = Args->CurrentPoint->Position.X;
    pointerPosition.y = Args->CurrentPoint->Position.Y;
}

void App::PointerReleased(CoreWindow^ Window, PointerEventArgs^ Arg) {
    PointerLocked = false;
}

void App::KeyDown(CoreWindow^ Window, KeyEventArgs^ Args) {
    VirtualKey key = Args->VirtualKey;
    if (key == (VirtualKey)87) {
        dt += SEN;
    }
    if (key == (VirtualKey)83) {
        dt -= SEN;
    }
    if (key == (VirtualKey)65) {
        ds -= SEN;
    }
    if (key == (VirtualKey)68) {
        ds += SEN;
    }
    if (key == (VirtualKey)13) {
        display.newSetOfPoints();
    }
}

void App::Closed(CoreWindow^ Window, CoreWindowEventArgs^ Args) {
    WindowClosed = true;
}