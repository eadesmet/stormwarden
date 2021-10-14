


internal void
ScreenToNDC(f32 ScreenCoord, f32 MaxValueInDimension)
{
    // NOTE(Eric): ScreenCoord == Pixel value in the window
    // NOTE(Eric): MaxValueInDimension == Window size in that direction
    
    // NOTE(Eric): Converting from Screen Space to NDC space
    // i.e. x[0,1920] y[0,1080] pixel value to NDC x[-1,1] y[-1,1]
    
    // TODO(Eric): Somehow the shader can do this for us, but I don't know how.
    
    f32 Result = Lerp(-1, 1, ScreenCoord/MaxValueInDimension);
    
    return Result;
}

internal void
v2ScreenToNDC(v2 ScreenCoord, v2 Max)
{
    Result = v2(ScreenToNDC(ScreenCoord.x, Max.x),
                ScreenToNDC(ScreenCoord.y, Max.y));
    
    return Result;
}


// TODO(Eric): Points in Meters to Screen

