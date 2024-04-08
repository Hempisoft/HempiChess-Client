# Projektdokumentation

## Inhaltsverzeichnis
- [Einführung](#introduction)
- [Funktionen](#features)
- [Klassen](#classes)
- [Bemerkenswerte Funktionen](#notable-functions)
- [Implementierung von Universitätskonzepten](#implementation-of-university-concepts)
- [Zukünftige Möglichkeiten](#future-possibilities)

## Einführung


## Merkmale
Wie in der [Einführung](#Einführung) erwähnt, beinhaltet das Projekt eine grafische Visualisierung von spielbarem Schach, unter Verwendung der [Win32-API](https://de.wikipedia.org/wiki/Windows_Application_Programming_Interface) und der [GdiPlus-Library](https://learn.microsoft.com/de-de/windows/win32/gdiplus/-gdiplus-gdi-start). 

Im Einzelnen handelt es sich um die folgenden Features:
- Spielbares Schach über Mauseingaben, mit dem Standard [FIDE-Schach-Regelwerk](https://www.fide.com/FIDE/handbook/LawsOfChess.pdf)
- Lebensqualitäts-Optionen 
    - Eine Einstellung, bei der sich das Brett dreht, je nachdem, welcher Spieler am Zug ist
    - Eine Einstellung für die visuelle Anzeige der Bewegungsoptionen für die ausgewählte Figur
    - Eine Rückgängig-Taste
- Eine Option zum Bearbeiten der [FEN](https://de.wikipedia.org/wiki/Forsyth-Edwards-Notation) des Brettes
- ? Ein Timer für zeitgesteuertes Schach ?

## Klassen
Das Projekt umfasst die folgenden Klassen:
- [Projekt](#project) 
- [Fenster](#window)
- [Paint](#paint)
- [Eingabe](#input)
- [Board](#board)
- [Stück](#stück)

Die gesamte Initialisierungsstruktur sieht wie folgt aus:

![Initialisierungsbaum](./Initialization%20tree.png)

### Projekt
Die Klasse Project enthält die Funktion main, die die Hauptinstanzen aller anderen Klassen erzeugt und verwaltet, sowie den Timer Thread.

### Window
Die Klasse Window erstellt das Fenster unter Verwendung der Win-32-API. Alle Interaktionen mit dem Fenster werden in der Windows-[Window-Prozedur](#window-procedure) aufgefangen, die alle notwendigen Informationen an die [Input-Klasse](#input) weiterleitet, wo die Verarbeitung der Interaktionen erfolgt. Die Window-Klasse verwaltet auch alle Aspekte des Programms, die die Window-API verwenden, und entscheidet, wann die [Paint](#paint)-Klasse aufgerufen werden muss.

### Paint
Die Paint-Klasse verwaltet alle Aktionen, die die gdiplus-Bibliothek benötigen, d.h. die Paint-Klasse ist für alle Ereignisse verantwortlich, die sich auf etwas Visuelles auf der Windows-GUI-Oberfläche beziehen. Alle sichtbaren Elemente des Fensters werden durch die Funktionen der Paint-Klasse erzeugt und auf das Windows-Grafik-Objekt gezeichnet.

### Eingabe
Die Input Klasse, wie in der [Window classes description](#window) erwähnt, handhabt die notwendigen Interaktionen mit dem Fenster, insbesondere die Mausklicks und Dragging-Ereignisse werden von den Funktionen der Input Klasse verwaltet. Die Input Klasse konvertiert die Koordinaten der Mausklicks in die angeklickten Brettfelder und gibt diese Informationen an die [Board Klasse](#board) weiter, wo die Spiellogik abgewickelt wird. 

### Brett
Die Board-Klasse steuert die Spiellogik des Schachspiels und speichert alle damit verbundenen Werte. 

### Stück
Die Klasse Piece prüft die Bewegungsregeln für die verschiedenen Figuren.

## Bemerkenswerte Funktionen

### WindowProc
Die Window-Prozedur wird jedes Mal aufgerufen, wenn ein Fensterereignis (aus der Win32-API) eintritt, z.B. ein Mausklick. Die Window-Prozedur unterscheidet den Typ des Ereignisses und ruft die Funktion auf, die für die Behandlung des Ereignisses verantwortlich ist. 

### DrawingProcedure
Die DrawingProcedure wird von der [Window Procedure](#windowproc) aufgerufen, wenn das [WM_PAINT Event](https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint) eintritt, welches in diesem Fall durch die [InvalidateRect Function](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-invalidaterect) ausgelöst wird, die durch das Dragging Event oder die Main-Funktionsschleife aufgerufen wird. Sie erzeugt ein gefälschtes HDC-Objekt, auf dessen Grafikobjekt die verschiedenen [Paint](#paint)-Funktionen die GUI zeichnen. Dieses gefälschte HDC-Objekt wird dann auf das Fenster gespiegelt. Diese komplizierte Prozedur ist notwendig, da beim Übermalen des letzten Frames auf das Hauptgrafikobjekt ein "Flacker"-Effekt auftritt. Indem man zuerst malt und danach die Grafik spiegelt, wird das Flackern verhindert und die Bewegung auf dem Fenster erscheint flüssig. Außerdem werden die Fenstergrafiken viel weniger aktualisiert, was die Belastung der GPU des Computers senkt.

### MovePiece
Die movePiece Funktion im Board 

### Move
Die Zugfunktion in der [piece class](#piece) wird von [movePiece](#movepiece) und [TestAvailableMoves](#testavailablemoves) aufgerufen, um die Korrektheit eines Zuges zu überprüfen. Dieser Code wurde direkt aus den Übungsblättern übernommen, nur modifiziert, damit das Brett als 64-Zeichen-Zeichenfolge ausgedrückt werden kann, anstatt als 8x8 2-dimensionales Zeichenfeld. Er wurde auch mit den Rochade- und En-Passant-Prüfungen aus dem Bonusblatt gemischt. 

### TestAvailableMoves
Die Funktion testAvailableMoves in der [Figurenklasse](#Stück) gibt einen Vektor mit allen möglichen Zügen für den angegebenen Spieler zurück. Dies wird sowohl für die Option "Zugoptionen anzeigen" als auch für die Spielende-Prüfungen verwendet, da keine verfügbaren Züge entweder ein Patt oder ein Schachmatt bedeuten. Diese Funktion ist eine Variation der Funktion testCheckmate, die ich in den Übungsblättern implementiert habe.

## Umsetzung der gelernten Konzepte

### Rekursion
Das Projekt nutzt die Rekursion in der [Zugfunktion](#move), um zu testen, ob eine Figur den Weg einer Dame, eines Turms oder eines Läufers blockiert, indem jeder Zug mit der Länge 1 in Zugrichtung getestet wird. 

### Modularität
Das Projekt verwendet Modularität, indem es die gleiche Art von Aufgaben in eigene Klassen aufteilt, wie im Unterabschnitt [Klassen](#classes) beschrieben. Alle nicht statischen Mitgliedsvariablen und Funktionen werden ebenfalls in eigenen Header-Dateien deklariert, um die Übersicht über die Klassen als Ganzes zu erleichtern. 

### Fehlerbehandlung
Bei der FEN-Eingabe kann der Benutzer eine beliebige Zeichenfolge in das FEN eingeben. Dies könnte zu Speicherlecks, Injektionen oder Abstürzen führen. Aus diesem Grund muss die Behauptung der FEN-Korrektheit frei von Exploits sein. Aus diesem Grund verwendet die Setup-Funktion in der [Board-Klasse](#board) insbesondere Errorhandling, um mögliche Schäden zu verhindern.

### Lambda-Funktion & Multithreading
Um das Timer-Feature für zeitgesteuerte Schachpartien einzubinden, wird in der Projektklasse eine Timer-Lambda-Funktion deklariert, die dann in einen separaten Thread gewickelt und neben dem Hauptthread laufen gelassen wird. Auf diese Weise kann der Timer sehr viel genauer arbeiten, denn wäre er in die Hauptspielschleife eingebunden, würden die unvorhersehbaren Zeitkosten des gesamten Programms die Genauigkeit des Timers beeinflussen. 

## Zukünftige Möglichkeiten
- Einbindung von Bots
- Einfache Implementierung von Schachvarianten
- Multiplayer über Socket-Verbindung