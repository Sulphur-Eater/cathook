/*
 * angles.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: nullifiedcat
 */
#include "common.hpp"
#include "angles.hpp"

namespace angles
{
angle_data_s data_[PLAYER_ARRAY_SIZE];

void angle_data_s::push(const Vector &angle)
{
    if (!angle.x && !angle.y)
        return;
    good                = true;
    angles[angle_index] = angle;
    if (++angle_index >= count)
        angle_index = 0;
    /*if (angle_count > 0) {
        int ai = angle_index - 2;
        if (ai < 0) ai = count - 1;
        float dx = std::abs(angles[ai].x - angle.x);
        float dy = std::abs(angles[ai].y - angle.y);
        if (sqrt(SQR(dx) + SQR(dy)) > 45.0f) {
            //logging::Info("%.2f %.2f %.2f", dx, dy, sqrt(SQR(dx) + SQR(dy)));
        }
    }*/
    if (angle_count < count)
        angle_count++;
}

float angle_data_s::deviation(int steps) const
{
    int j    = angle_index - 2;
    int k    = j + 1;
    float hx = 0, hy = 0;
    for (int i = 0; i < steps && i < angle_count; i++)
    {
        if (j < 0)
            j = count + j;
        if (k < 0)
            k = count + k;

        float dev_x = std::abs(angles[k].x - angles[j].x);
        float dev_y = std::abs(angles[k].y - angles[j].y);
        if (dev_x > hx)
            hx = dev_x;
        if (dev_y > hy)
            hy = dev_y;

        // logging::Info("1: %.2f %.2f | 2: %.2f %.2f | dev: %.2f",
        // angles[k].x, angles[k].y, angles[j].x, angles[j].y, sqrt(dev_x *
        // dev_x + dev_y * dev_y));

        --j;
        --k;
    }
    if (hy > 180)
        hy = 360 - hy;
    return sqrt(SQR(hx) + SQR(hy));
}

angle_data_s &data_idx(int index)
{
    if (index < 1 || index > MAX_PLAYERS)
    {
        throw std::out_of_range("Angle table entity index out of range");
    }
    return data_[index];
}

angle_data_s &data(const CachedEntity *entity)
{
    return data_idx(entity->m_IDX);
}

void Update()
{
    for (const auto &ent : entity_cache::player_cache)
    {
        auto &d = data_idx(ent->m_IDX);
        if (CE_GOOD(ent) && !CE_BYTE(ent, netvar.iLifeState))
        {
            if (!d.good)
                memset(&d, 0, sizeof(angle_data_s));

            if (ent->m_IDX == g_IEngine->GetLocalPlayer())
                d.push(current_user_cmd->viewangles);
            else
                d.push(CE_VECTOR(ent, netvar.m_angEyeAngles));
        }
        else
            d.good = false;
    }
}
} // namespace angles
