/*
 * allowedlayersdialog.h
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

#include <QDialog>

class QLineEdit;
class QListWidget;
class QTableWidget;

namespace Tiled {

/**
 * A dialog for choosing the names of the layers on which a tileset may be
 * used. The result is stored in the "allowedLayers" custom property of the
 * tileset, as a comma-separated list.
 */
class AllowedLayersDialog : public QDialog
{
    Q_OBJECT

public:
    AllowedLayersDialog(const QString &tilesetName,
                        const QStringList &layerNames,
                        const QStringList &allowedLayers,
                        QWidget *parent = nullptr);

    QStringList allowedLayers() const;

private:
    void addName();
    void addItem(const QString &name, bool checked);
    bool containsName(const QString &name) const;

    QListWidget *mList;
    QLineEdit *mNewName;
};

/**
 * A dialog giving an overview of the layers on which each tileset may be used,
 * as a table with a row per tileset and a column per layer name. Checking a
 * cell allows the tileset on layers with that name. Tilesets without any
 * checked cell can be used on any layer.
 */
class AllowedLayersOverviewDialog : public QDialog
{
    Q_OBJECT

public:
    AllowedLayersOverviewDialog(const QStringList &tilesetNames,
                                const QStringList &layerNames,
                                const QList<QStringList> &allowedLayers,
                                QWidget *parent = nullptr);

    /**
     * Returns the allowed layers for each tileset, in the order in which the
     * tilesets were given.
     */
    QList<QStringList> allowedLayers() const;

private:
    void addLayerColumn(const QString &name);
    void addNewLayerColumn();
    int columnOf(const QString &name) const;
    void updateRestriction(int row);
    void toggleColumn(int column);

    QTableWidget *mTable;
    QLineEdit *mNewName;
    bool mUpdating = false;
};

/**
 * A dialog that lists the tiles that are on a layer on which their tileset is
 * not allowed. Activating a row emits entryActivated, so that the owner can
 * select the layer and the tiles.
 */
class AllowedLayersCheckDialog : public QDialog
{
    Q_OBJECT

public:
    struct Entry {
        QString layerPath;
        QString tilesetName;
        int tileCount;
        QString allowedLayers;
    };

    AllowedLayersCheckDialog(const QList<Entry> &entries, QWidget *parent = nullptr);

signals:
    void entryActivated(int index);

private:
    QTableWidget *mTable;
};

} // namespace Tiled
