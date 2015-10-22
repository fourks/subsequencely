
#include "slider.h"

void slider_init(Slider* s, Orientation orientation, u8 position,
                 const u8* color, u8 resolution, s16 offset, s16 value)
{
    s->orientation = orientation;
    s->position = position;
    s->color = color;
    s->resolution = resolution;
    s->offset = offset;
    s->value = value - offset;
}

void slider_set_value(Slider* s, s16 value)
{
    s->value = value - s->offset;
}

s16 slider_get_value(Slider* s)
{
    return s->value + s->offset;
}

u8 slider_handle_press(Slider* s, u8 index, u8 value)
{
    u8 x, y;
    if (value == 0 || !index_to_pad(index, &x, &y))
    {
        return 0;
    }

    u8 pad;

    if (s->orientation == VERTICAL && s->position == x)
    {
        pad = y;
    }
    else if (s->orientation == HORIZONTAL && s->position == y)
    {
        pad = x;
    }
    else
    {
        return 0;
    }

    // All the pads before the one that was pressed are filled up, so find
    // their value, then find how much of the pressed pad is filled up based
    // on how hard it was pressed.
    s->value = pad * s->resolution;

    // The harder you press, the further away from 0 the value should be,
    // so if there's a negative offset and the pad that was pressed is on
    // the negative side of 0, then reverse the velocity.
    if (s->value + s->offset < 0)
    {
        value = 127 - value;
    }

    // Each pad has a number of possible values, determined by s->resolution,
    // so figure out what fraction of them are filled and make sure at least
    // one is filled, so that the value doesn't creep into the range of the
    // previous pad.
    s->value += max(1, s->resolution * value / 127);

    return 1;
}

void slider_draw(Slider* s)
{
    u8 x = 0;
    u8 y = 0;
    u8* coord;

    if (s->orientation == VERTICAL)
    {
        x = s->position;
        coord = &y;
    }
    else
    {
        y = s->position;
        coord = &x;
    }

    // For positive offset (unipolar) sliders, all the pads before the value
    // are filled, but for negative offsets, only the pads between the one
    // that 0 is in and the one the value lies in are filled.
    u8 range_start = 0;
    u8 range_end = 0;
    u8 zero_point = -s->offset / s->resolution - 1;
    u8 value_point = (s->value - 1) / s->resolution;

    if (s->offset >= 0)
    {
        range_end = value_point;
    }
    else if (s->value + s->offset <= 0)
    {
        range_start = value_point;
        range_end = zero_point;
    }
    else 
    {
        range_start = zero_point;
        range_end = value_point;
    }

    u8 prev_value = 0;

    for (*coord = 0; *coord < GRID_SIZE; (*coord)++)
    {
        const u8* color = off_color;
        if (*coord >= range_start
            && *coord <= range_end)
        {
            color = s->color;
        }

        u8 dimness = 0;
        if (*coord == value_point)
        {
            // Figure out how much of the resolution of this pad is filled.
            // Reverse the direction for negative values just like in
            // handle_press.
            u8 pad_value = s->value - prev_value;
            if (s->value + s->offset <= 0)
            {
                pad_value = s->resolution - pad_value + 1;
            }

            // Fit the different possible resolutions into 5 brightness levels,
            // so that the lights can't go all the way off.
            dimness = 5 * (s->resolution - pad_value) / s->resolution;
        }

        plot_pad_dim(coord_to_index(x, y), color, dimness);

        prev_value += s->resolution;
    }
}
