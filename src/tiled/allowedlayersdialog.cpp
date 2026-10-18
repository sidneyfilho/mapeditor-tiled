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
#include <QTableWidget>
#include <QHeaderView>
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

AllowedLayersOverviewDialog::AllowedLayersOverviewDialog(const QStringList &tilesetNames,
                                                         const QStringList &layerNames,
                                                         const QList<QStringList> &allowedLayers,
                                                         QWidget *parent)
    : QDialog(parent)
    , mTable(new QTableWidget(tilesetNames.size(), 1, this))
    , mNewName(new QLineEdit(this))
{
    setWindowTitle(tr("All Allowed Layers"));

    auto description = new QLabel(tr("Each row is a tileset and each column a layer name. "
                                     "Check the layers on which a tileset may be used. "
                                     "Tilesets without any checked layer can be used on any layer. "
                                     "Click a layer name to toggle its whole column."),
                                  this);
    description->setWordWrap(true);

    mTable->setVerticalHeaderLabels(tilesetNames);
    mTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Restriction")));
    mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTable->horizontalHeader()->setSectionsClickable(true);
    mTable->setSelectionMode(QAbstractItemView::NoSelection);

    for (int row = 0; row < tilesetNames.size(); ++row) {
        auto item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled);
        mTable->setItem(row, 0, item);
    }

    mUpdating = true;

    for (const QString &name : layerNames)
        addLayerColumn(name);
    for (const QStringList &allowed : allowedLayers) {
        for (const QString &name : allowed)
            addLayerColumn(name);
    }

    for (int row = 0; row < allowedLayers.size(); ++row) {
        for (const QString &name : allowedLayers.at(row))
            mTable->item(row, columnOf(name))->setCheckState(Qt::Checked);
    }

    for (int row = 0; row < mTable->rowCount(); ++row)
        updateRestriction(row);

    mUpdating = false;

    mNewName->setPlaceholderText(tr("Other layer name"));
    mNewName->setClearButtonEnabled(true);

    auto addButton = new QPushButton(tr("Add Layer"), this);
    addButton->setAutoDefault(false);

    auto addLayout = new QHBoxLayout;
    addLayout->addWidget(mNewName);
    addLayout->addWidget(addButton);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(description);
    layout->addWidget(mTable);
    layout->addLayout(addLayout);
    layout->addWidget(buttons);

    connect(mTable, &QTableWidget::itemChanged, this, [this] (QTableWidgetItem *item) {
        if (!mUpdating && item->column() > 0)
            updateRestriction(item->row());
    });
    connect(mTable->horizontalHeader(), &QHeaderView::sectionClicked, this, [this] (int column) {
        if (column > 0)
            toggleColumn(column);
    });
    connect(addButton, &QPushButton::clicked, this, &AllowedLayersOverviewDialog::addNewLayerColumn);
    connect(mNewName, &QLineEdit::returnPressed, this, &AllowedLayersOverviewDialog::addNewLayerColumn);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    resize(760, 460);
}

QList<QStringList> AllowedLayersOverviewDialog::allowedLayers() const
{
    QList<QStringList> result;

    for (int row = 0; row < mTable->rowCount(); ++row) {
        QStringList allowed;
        for (int column = 1; column < mTable->columnCount(); ++column) {
            if (mTable->item(row, column)->checkState() == Qt::Checked)
                allowed.append(mTable->horizontalHeaderItem(column)->text());
        }
        result.append(allowed);
    }

    return result;
}

void AllowedLayersOverviewDialog::addLayerColumn(const QString &name)
{
    if (columnOf(name) != -1)
        return;

    const bool wasUpdating = mUpdating;
    mUpdating = true;

    const int column = mTable->columnCount();
    mTable->insertColumn(column);
    mTable->setHorizontalHeaderItem(column, new QTableWidgetItem(name));

    for (int row = 0; row < mTable->rowCount(); ++row) {
        auto item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        mTable->setItem(row, column, item);
    }

    mUpdating = wasUpdating;
}

void AllowedLayersOverviewDialog::addNewLayerColumn()
{
    const QString name = mNewName->text().trimmed().remove(QLatin1Char(','));
    if (name.isEmpty())
        return;

    addLayerColumn(name);
    mNewName->clear();
}

int AllowedLayersOverviewDialog::columnOf(const QString &name) const
{
    for (int column = 1; column < mTable->columnCount(); ++column) {
        if (mTable->horizontalHeaderItem(column)->text().compare(name, Qt::CaseInsensitive) == 0)
            return column;
    }

    return -1;
}

void AllowedLayersOverviewDialog::updateRestriction(int row)
{
    bool restricted = false;
    for (int column = 1; column < mTable->columnCount() && !restricted; ++column)
        restricted = mTable->item(row, column)->checkState() == Qt::Checked;

    const bool wasUpdating = mUpdating;
    mUpdating = true;

    QTableWidgetItem *item = mTable->item(row, 0);
    item->setText(restricted ? tr("Restricted") : tr("Any layer"));
    item->setForeground(restricted ? QBrush() : QBrush(Qt::gray));

    mUpdating = wasUpdating;
}

void AllowedLayersOverviewDialog::toggleColumn(int column)
{
    bool anyUnchecked = false;
    for (int row = 0; row < mTable->rowCount(); ++row)
        anyUnchecked |= mTable->item(row, column)->checkState() != Qt::Checked;

    const Qt::CheckState state = anyUnchecked ? Qt::Checked : Qt::Unchecked;
    for (int row = 0; row < mTable->rowCount(); ++row)
        mTable->item(row, column)->setCheckState(state);
}

AllowedLayersCheckDialog::AllowedLayersCheckDialog(const QList<Entry> &entries, QWidget *parent)
    : QDialog(parent)
    , mTable(new QTableWidget(entries.size(), 4, this))
{
    setWindowTitle(tr("Tiles on Wrong Layers"));
    setAttribute(Qt::WA_DeleteOnClose);

    auto description = new QLabel(tr("These layers contain tiles of tilesets that are not allowed on them. "
                                     "Double-click a row to select the layer and the wrong tiles, "
                                     "then press Delete to erase them or move them to a suitable layer."),
                                  this);
    description->setWordWrap(true);

    mTable->setHorizontalHeaderLabels({ tr("Layer"), tr("Tileset"), tr("Tiles"), tr("Allowed layers") });
    mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mTable->verticalHeader()->setVisible(false);
    mTable->horizontalHeader()->setStretchLastSection(true);

    for (int row = 0; row < entries.size(); ++row) {
        const Entry &entry = entries.at(row);
        mTable->setItem(row, 0, new QTableWidgetItem(entry.layerPath));
        mTable->setItem(row, 1, new QTableWidgetItem(entry.tilesetName));
        mTable->setItem(row, 2, new QTableWidgetItem(QString::number(entry.tileCount)));
        mTable->setItem(row, 3, new QTableWidgetItem(entry.allowedLayers));
    }
    mTable->resizeColumnsToContents();

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(description);
    layout->addWidget(mTable);
    layout->addWidget(buttons);

    connect(mTable, &QTableWidget::cellDoubleClicked, this, [this] (int row) {
        emit entryActivated(row);
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);

    resize(640, 360);
}

} // namespace Tiled
