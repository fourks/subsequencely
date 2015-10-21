

#include "session.h"

void session_draw(Sequencer* sr)
{
    for (u8 y = 0; y < GRID_SIZE; y++)
    {
        u8 seq_i = row_to_seq(y);
        Sequence* s = &sr->sequences[seq_i];

        for (u8 x = 0; x < GRID_SIZE; x++)
        {
            u8 dimness = 5 * (s->playhead / STEPS_PER_PAD != x);
            plot_pad_dim(coord_to_index(x, y), sequence_colors[seq_i], dimness);
        }
    }
}


u8 session_handle_press(Sequencer* sr, u8 index, u8 value)
{
    u8 x, y;
    if (!index_to_pad(index, &x, &y))
    {
        return 0;
    }

    u8 seq_i = row_to_seq(y);

    if (modifier_held(LP_SHIFT))
    {
        
    }
    else
    {
        sequence_queue_at(&sr->sequences[seq_i], x * STEPS_PER_PAD);
    }

    return 1;
}