// $Id$

// Copyright by Johannes Sixt
// This file is under GPL, the GNU General Public Licence

#ifndef SOURCEWND_H
#define SOURCEWND_H

#include <qpixmap.h>
#include "textvw.h"
#include "dbgdriver.h"

// forward declarations
class KDebugger;
struct DbgAddr;

class SourceWindow : public KTextView
{
    Q_OBJECT
public:
    SourceWindow(const char* fileName, QWidget* parent, const char* name);
    ~SourceWindow();
    
    void loadFile();
    void reloadFile();
    bool fileNameMatches(const QString& other);
    void scrollTo(int lineNo, const DbgAddr& address);
    const QString& fileName() const { return m_fileName; }
    void updateLineItems(const KDebugger* dbg);
    void setPC(bool set, int lineNo, const DbgAddr& address, int frameNo);
    enum FindDirection { findForward = 1, findBackward = -1 };
    void find(const QString& text, bool caseSensitive, FindDirection dir);
    bool wordAtPoint(const QPoint& p, QString& word, QRect& r);
    /**
     * Translates row number (zero-based) to zero-based source line number.
     * If sourceRow is non-zero, it is filled with the source code row
     * belonging to the line number.
     */
    int rowToLine(int row, int* sourceRow = 0);
    /** Translates zero-based source line number to row number (zero-based) */
    int lineToRow(int line);
    /** Is the row disassembled? */
    bool isRowExpanded(int row);
    /** Does the row show disassembled code? */
    bool isRowDisassCode(int row);

    /** lineNo is zero-based */
    void disassembled(int lineNo, const QList<DisassembledCode>& disass);

    void activeLine(int& lineNo, DbgAddr& address);

protected:
    virtual int textCol() const;
    virtual int cellWidth(int col);
    virtual void paintCell(QPainter* p, int row, int col);
    virtual void mousePressEvent(QMouseEvent* ev);
    virtual void paletteChange(const QPalette&);
    void updateLineItem(int i);
    void expandRow(int row);
    void collapseRow(int row);
    void scrollToRow(int row);
    /** translates (0-based) line number plus a code address into a row number */
    int lineToRow(int row, const DbgAddr& address);

signals:
    void clickedLeft(const QString&, int, const DbgAddr& address, bool);
    void clickedMid(const QString&, int, const DbgAddr& address);
    void clickedRight(const QPoint &);
    void disassemble(const QString&, int);
    void expanded(int lineNo);		/* source lineNo has been expanded */
    void collapsed(int lineNo);		/* source lineNo has been collapsed */

protected:
    QString m_fileName;
    enum LineItem { liPC = 1, liPCup = 2,
	liBP = 4, liBPdisabled = 8, liBPtemporary = 16,
	liBPconditional = 32,
	liBPany = liBP|liBPdisabled|liBPtemporary|liBPconditional
    };

    struct SourceLine {
	QString code;			/* a line of text */
	ValArray<QString> disass;	/* its disassembled code */
	ValArray<DbgAddr> disassAddr;	/* the addresses thereof */
	bool canDisass;			/* if line can be disassembled */
	SourceLine() : canDisass(true) { }
	int findAddressRowOffset(const DbgAddr& address) const;
    };
    ValArray<SourceLine> m_sourceCode;

    ValArray<int> m_rowToLine;		/* source line number for each row */
    ValArray<uchar> m_lineItems;
    QPixmap m_pcinner;			/* PC at innermost frame */
    QPixmap m_pcup;			/* PC at frame up the stack */
    QPixmap m_brkena;			/* enabled breakpoint */
    QPixmap m_brkdis;			/* disabled breakpoint */
    QPixmap m_brktmp;			/* temporary breakpoint marker */
    QPixmap m_brkcond;			/* conditional breakpoint marker */
};

#endif // SOURCEWND_H