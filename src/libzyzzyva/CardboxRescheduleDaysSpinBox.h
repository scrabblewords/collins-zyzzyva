//---------------------------------------------------------------------------
// CardboxRescheduleDaysSpinBox.h
//
// A widget for rescheduling words by a number of days within the cardbox
// system.
//
// Copyright 2016 Twilight Century Computing.
// Copyright 2012 North American SCRABBLE Players Association.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#ifndef ZYZZYVA_CARDBOX_RESCHEDULE_DAYS_SPINBOX_H
#define ZYZZYVA_CARDBOX_RESCHEDULE_DAYS_SPINBOX_H

#include <QSpinBox>

class CardboxRescheduleDaysSpinBox : public QSpinBox
{
    Q_OBJECT
    public:
    CardboxRescheduleDaysSpinBox(QWidget* parent = 0);
    ~CardboxRescheduleDaysSpinBox() { }

    protected:
    virtual QString textFromValue(int value) const;
};

#endif // ZYZZYVA_CARDBOX_RESCHEDULE_DAYS_SPINBOX_H

