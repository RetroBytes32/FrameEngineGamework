#include <GameEngineFramework/Engine/types/viewport.h>

Viewport::Viewport() : 
    x(0),
    y(0),
    w(0),
    h(0)
{
}

Viewport::Viewport(int left, int top, int width, int height) : 
    x(left),
    y(top),
    w(width),
    h(height)
{
}

