/*
 * allowedlayersdialog.cpp
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

#include "allowedlayersdialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Tiled {

AllowedLayersDialog::AllowedLayersDialog(const QString &tilesetName,
                                         const QStringList &layerNames,
                                         const QStringList &allowedLayers,
                                         QWidget *parent)
    : QDialog(parent)
    , mList(new QListWidget(this))
    , mNewName(new QLineEdit(this))
{
    setWindowTitle(tr("Allowed Layers"));

    auto description = new QLabel(tr("Choose the layers on which tiles of the tileset "
                                     "'%1' may be painted. When no layer is checked, "
                                     "the tileset can be used on any layer.").arg(tilesetName),
                                  this);
    description->setWordWrap(true);

    // Names of the layers in the map, followed by names that were already
    // allowed but do not occur in the map
    for (const QString &name : layerNames)
        addItem(name, allowedLayers.contains(name, Qt::CaseInsensitive));
    for (const QString &name : allowedLayers) {
        if (!containsName(name))
            addItem(name, true);
    }

    mNewName->setPlaceholderText(tr("Other layer name"));
    mNewName->setClearButtonEnabled(true);

    auto addButton = new QPushButton(tr("Add"), this);
    addButton->setDefault(false);
    addButton->setAutoDefault(false);

    auto clearButton = new QPushButton(tr("Uncheck All"), this);
    clearButton->setAutoDefault(false);

    auto addLayout = new QHBoxLayout;
    addLayout->addWidget(mNewName);
    addLayout->addWidget(addButton);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->addButton(clearButton, QDialogButtonBox::ResetRole);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(description);
    layout->addWidget(mList);
    layout->addLayout(addLayout);
    layout->addWidget(buttons);

    connect(addButton, &QPushButton::clicked, this, &AllowedLayersDialog::addName);
    connect(mNewName, &QLineEdit::returnPressed, this, &AllowedLayersDialog::addName);
    connect(clearButton, &QPushButton::clicked, this, [this] {
        for (int i = 0; i < mList->count(); ++i)
            mList->item(i)->setCheckState(Qt::Unchecked);
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    resize(360, 420);
}

QStringList AllowedLayersDialog::allowedLayers() const
{
    QStringList result;

    for (int i = 0; i < mList->count(); ++i) {
        const QListWidgetItem *item = mList->item(i);
        if (item->checkState() == Qt::Checked)
            result.append(item->text());
    }

    return result;
}

void AllowedLayersDialog::addName()
{
    const QString name = mNewName->text().trimmed().remove(QLatin1Char(','));
    if (name.isEmpty())
        return;

    if (!containsName(name))
        addItem(name, true);

    mNewName->clear();
}

void AllowedLayersDialog::addItem(const QString &name, bool checked)
{
    auto item = new QListWidgetItem(name, mList);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

bool AllowedLayersDialog::containsName(const QString &name) const
{
    for (int i = 0; i < mList->count(); ++i) {
        if (mList->item(i)->text().compare(name, Qt::CaseInsensitive) == 0)
            return true;
    }

    return false;
}

} // namespace Tiled
