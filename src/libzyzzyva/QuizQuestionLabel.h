//---------------------------------------------------------------------------
// QuizQuestionLabel.h
//
// A class derived from QLabel, used to display quiz questions.  This class
// currently does nothing special.  It only exists so objects of this class
// can be distinguished from other QLabel objects when applying font settings.
//
// Copyright 2015-2016 Twilight Century Computing.
// Copyright 2006-2012 North American SCRABBLE Players Association.
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

#ifndef ZYZZYVA_QUIZ_QUESTION_LABEL_H
#define ZYZZYVA_QUIZ_QUESTION_LABEL_H

#include <QLabel>

class QuizQuestionLabel : public QLabel
{
    Q_OBJECT
    public:
    QuizQuestionLabel(QWidget* parent = 0, Qt::WindowFlags f = 0)
        : QLabel(parent, f) { }
    QuizQuestionLabel(const QString& text, QWidget* parent = 0,
                      Qt::WindowFlags f = 0)
        : QLabel(text, parent, f) { }

    virtual ~QuizQuestionLabel() { }
};

#endif // ZYZZYVA_QUIZ_QUESTION_LABEL_H
