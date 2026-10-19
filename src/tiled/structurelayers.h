/*
 * structurelayers.h
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "grouplayer.h"
#include "layer.h"
#include "map.h"

#include <QString>

namespace Tiled {

/**
 * Structures are buildings that evolve through levels (a house, a barn...).
 *
 * A structure is a top-level group layer with the class "Estrutura". It
 * contains one group per level, and each level contains groups named after
 * the floors of the map ("layer5", "layer6"...) holding the usual layers:
 *
 *   Casa              (class "Estrutura")
 *     Casa 1          (a level)
 *       layer5        (drawn as part of the floor "layer5")
 *         floor, items
 *       layer6
 *         items
 *     Casa 2
 *       ...
 *
 * A structure is not a floor itself. Its floor groups belong to the top-level
 * floor of the same name.
 */
namespace StructureLayers {

inline const QString &structureClassName()
{
    static const QString name = QStringLiteral("Estrutura");
    return name;
}

inline Layer *topLevelLayer(Layer *layer)
{
    while (layer->parentLayer())
        layer = layer->parentLayer();
    return layer;
}

inline bool isStructure(const Layer *layer)
{
    return layer && !layer->parentLayer() && layer->isGroupLayer()
            && layer->className() == structureClassName();
}

/**
 * Returns whether the given layer is a level of a structure (a group directly
 * inside a structure).
 */
inline bool isStructureLevel(const Layer *layer)
{
    return layer && layer->isGroupLayer() && isStructure(layer->parentLayer());
}

/**
 * Returns the floor group of a structure that contains the given layer (or is
 * the given layer), or nullptr when the layer is not inside a structure floor.
 */
inline Layer *structureFloorGroup(Layer *layer)
{
    while (layer) {
        GroupLayer *parent = layer->parentLayer();
        if (parent && isStructureLevel(parent))
            return layer;
        layer = parent;
    }
    return nullptr;
}

/**
 * Returns the index of the top-level floor with the given name, ignoring
 * structures, or -1 when there is no such floor.
 */
inline int floorIndexByName(const Map *map, const QString &name)
{
    const auto &layers = map->layers();
    for (int i = 0; i < layers.size(); ++i) {
        const Layer *layer = layers.at(i);
        if (!isStructure(layer) && layer->name() == name)
            return i;
    }
    return -1;
}

/**
 * Returns the index of the top-level floor the given layer belongs to. Layers
 * inside a structure floor group belong to the floor of the same name.
 * Returns -1 for structures and levels themselves, or unknown floors.
 */
inline int floorIndexOf(Layer *layer)
{
    if (Layer *floorGroup = structureFloorGroup(layer))
        return floorIndexByName(layer->map(), floorGroup->name());

    Layer *top = topLevelLayer(layer);
    if (isStructure(top))
        return -1;
    return top->siblingIndex();
}

} // namespace StructureLayers
} // namespace Tiled
