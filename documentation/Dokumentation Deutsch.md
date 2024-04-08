# Projektdokumentation

## Inhaltsverzeichnis
- [Einf�hrung](#introduction)
- [Funktionen](#features)
- [Klassen](#classes)
- [Bemerkenswerte Funktionen](#notable-functions)
- [Implementierung von Universit�tskonzepten](#implementation-of-university-concepts)
- [Zuk�nftige M�glichkeiten](#future-possibilities)

## Einf�hrung


## Merkmale
Wie in der [Einf�hrung](#Einf�hrung) erw�hnt, beinhaltet das Projekt eine grafische Visualisierung von spielbarem Schach, unter Verwendung der [Win32-API](https://de.wikipedia.org/wiki/Windows_Application_Programming_Interface) und der [GdiPlus-Library](https://learn.microsoft.com/de-de/windows/win32/gdiplus/-gdiplus-gdi-start). 

Im Einzelnen handelt es sich um die folgenden Features:
- Spielbares Schach �ber Mauseingaben, mit dem Standard [FIDE-Schach-Regelwerk](https://www.fide.com/FIDE/handbook/LawsOfChess.pdf)
- Lebensqualit�ts-Optionen 
    - Eine Einstellung, bei der sich das Brett dreht, je nachdem, welcher Spieler am Zug ist
    - Eine Einstellung f�r die visuelle Anzeige der Bewegungsoptionen f�r die ausgew�hlte Figur
    - Eine R�ckg�ngig-Taste
- Eine Option zum Bearbeiten der [FEN](https://de.wikipedia.org/wiki/Forsyth-Edwards-Notation) des Brettes
- ? Ein Timer f�r zeitgesteuertes Schach ?

## Klassen
Das Projekt umfasst die folgenden Klassen:
- [Projekt](#project) 
- [Fenster](#window)
- [Paint](#paint)
- [Eingabe](#input)
- [Board](#board)
- [St�ck](#st�ck)

Die gesamte Initialisierungsstruktur sieht wie folgt aus:

![Initialisierungsbaum](./Initialization%20tree.png)

### Projekt
Die Klasse Project enth�lt die Funktion main, die die Hauptinstanzen aller anderen Klassen erzeugt und verwaltet, sowie den Timer Thread.

### Window
Die Klasse Window erstellt das Fenster unter Verwendung der Win-32-API. Alle Interaktionen mit dem Fenster werden in der Windows-[Window-Prozedur](#window-procedure) aufgefangen, die alle notwendigen Informationen an die [Input-Klasse](#input) weiterleitet, wo die Verarbeitung der Interaktionen erfolgt. Die Window-Klasse verwaltet auch alle Aspekte des Programms, die die Window-API verwenden, und entscheidet, wann die [Paint](#paint)-Klasse aufgerufen werden muss.

### Paint
Die Paint-Klasse verwaltet alle Aktionen, die die gdiplus-Bibliothek ben�tigen, d.h. die Paint-Klasse ist f�r alle Ereignisse verantwortlich, die sich auf etwas Visuelles auf der Windows-GUI-Oberfl�che beziehen. Alle sichtbaren Elemente des Fensters werden durch die Funktionen der Paint-Klasse erzeugt und auf das Windows-Grafik-Objekt gezeichnet.

### Eingabe
Die Input Klasse, wie in der [Window classes description](#window) erw�hnt, handhabt die notwendigen Interaktionen mit dem Fenster, insbesondere die Mausklicks und Dragging-Ereignisse werden von den Funktionen der Input Klasse verwaltet. Die Input Klasse konvertiert die Koordinaten der Mausklicks in die angeklickten Brettfelder und gibt diese Informationen an die [Board Klasse](#board) weiter, wo die Spiellogik abgewickelt wird. 

### Brett
Die Board-Klasse steuert die Spiellogik des Schachspiels und speichert alle damit verbundenen Werte. 

### St�ck
Die Klasse Piece pr�ft die Bewegungsregeln f�r die verschiedenen Figuren.

## Bemerkenswerte Funktionen

### WindowProc
Die Window-Prozedur wird jedes Mal aufgerufen, wenn ein Fensterereignis (aus der Win32-API) eintritt, z.B. ein Mausklick. Die Window-Prozedur unterscheidet den Typ des Ereignisses und ruft die Funktion auf, die f�r die Behandlung des Ereignisses verantwortlich ist. 

### DrawingProcedure
Die DrawingProcedure wird von der [Window Procedure](#windowproc) aufgerufen, wenn das [WM_PAINT Event](https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint) eintritt, welches in diesem Fall durch die [InvalidateRect Function](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-invalidaterect) ausgel�st wird, die durch das Dragging Event oder die Main-Funktionsschleife aufgerufen wird. Sie erzeugt ein gef�lschtes HDC-Objekt, auf dessen Grafikobjekt die verschiedenen [Paint](#paint)-Funktionen die GUI zeichnen. Dieses gef�lschte HDC-Objekt wird dann auf das Fenster gespiegelt. Diese komplizierte Prozedur ist notwendig, da beim �bermalen des letzten Frames auf das Hauptgrafikobjekt ein "Flacker"-Effekt auftritt. Indem man zuerst malt und danach die Grafik spiegelt, wird das Flackern verhindert und die Bewegung auf dem Fenster erscheint fl�ssig. Au�erdem werden die Fenstergrafiken viel weniger aktualisiert, was die Belastung der GPU des Computers senkt.

### MovePiece
Die movePiece Funktion im Board 

### Move
Die Zugfunktion in der [piece class](#piece) wird von [movePiece](#movepiece) und [TestAvailableMoves](#testavailablemoves) aufgerufen, um die Korrektheit eines Zuges zu �berpr�fen. Dieser Code wurde direkt aus den �bungsbl�ttern �bernommen, nur modifiziert, damit das Brett als 64-Zeichen-Zeichenfolge ausgedr�ckt werden kann, anstatt als 8x8 2-dimensionales Zeichenfeld. Er wurde auch mit den Rochade- und En-Passant-Pr�fungen aus dem Bonusblatt gemischt. 

### TestAvailableMoves
Die Funktion testAvailableMoves in der [Figurenklasse](#St�ck) gibt einen Vektor mit allen m�glichen Z�gen f�r den angegebenen Spieler zur�ck. Dies wird sowohl f�r die Option "Zugoptionen anzeigen" als auch f�r die Spielende-Pr�fungen verwendet, da keine verf�gbaren Z�ge entweder ein Patt oder ein Schachmatt bedeuten. Diese Funktion ist eine Variation der Funktion testCheckmate, die ich in den �bungsbl�ttern implementiert habe.

## Umsetzung der gelernten Konzepte

### Rekursion
Das Projekt nutzt die Rekursion in der [Zugfunktion](#move), um zu testen, ob eine Figur den Weg einer Dame, eines Turms oder eines L�ufers blockiert, indem jeder Zug mit der L�nge 1 in Zugrichtung getestet wird. 

### Modularit�t
Das Projekt verwendet Modularit�t, indem es die gleiche Art von Aufgaben in eigene Klassen aufteilt, wie im Unterabschnitt [Klassen](#classes) beschrieben. Alle nicht statischen Mitgliedsvariablen und Funktionen werden ebenfalls in eigenen Header-Dateien deklariert, um die �bersicht �ber die Klassen als Ganzes zu erleichtern. 

### Fehlerbehandlung
Bei der FEN-Eingabe kann der Benutzer eine beliebige Zeichenfolge in das FEN eingeben. Dies k�nnte zu Speicherlecks, Injektionen oder Abst�rzen f�hren. Aus diesem Grund muss die Behauptung der FEN-Korrektheit frei von Exploits sein. Aus diesem Grund verwendet die Setup-Funktion in der [Board-Klasse](#board) insbesondere Errorhandling, um m�gliche Sch�den zu verhindern.

### Lambda-Funktion & Multithreading
Um das Timer-Feature f�r zeitgesteuerte Schachpartien einzubinden, wird in der Projektklasse eine Timer-Lambda-Funktion deklariert, die dann in einen separaten Thread gewickelt und neben dem Hauptthread laufen gelassen wird. Auf diese Weise kann der Timer sehr viel genauer arbeiten, denn w�re er in die Hauptspielschleife eingebunden, w�rden die unvorhersehbaren Zeitkosten des gesamten Programms die Genauigkeit des Timers beeinflussen. 

## Zuk�nftige M�glichkeiten
- Einbindung von Bots
- Einfache Implementierung von Schachvarianten
- Multiplayer �ber Socket-Verbindung