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

} // namespace Tiled
