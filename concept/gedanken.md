# Gedanken

## Go

* Spielfeld und Regeln
    * Datenstrukturen
    * Status des Feldes
        * Positionen der Steine
        * Ketten und Freiheiten
    * Prüfen auf ungültige Züge
    * Spielende
        * doppeltes Passen
        * Zeitlimit
        * keine validen Züge möglich
* Aufzeichnung
    * Beurteilung von Gelerntem
    * SGF (Smart Game Format)
* Kommunikation
    * Spiel gegen Menschen / andere Bots
    * GTP (Go Text Protocol)

Möglichkeiten

1. vorhandene Implementierungen (z.B. GNU Go) verwenden
2. eigene schreiben (ggf. an obiges angelehnt)

Vorschläge:

* möglichst einfache Regeln


## Neurale Netzwerke

Fragen:

* Größe des Netzwerks
* Inputs
* Was passiert, wenn Züge ungültig wären?
    * direktes Feedback?
* Möglichkeit zu passen
* Feedback
    * nur durch Vererbung
    * nach Spiel
    * nach Zug
* zusätzliches Training
* Dateiformat

Implementation:

* eigene
* Benutzung von Frameworks (z.B. OpenNN)
    * möglicherweise komplex, mehr Funktionalität als notwendig 


## Genetische Algorithmen

* Wie viel Zufall bzw. welche Verteilung des Genmaterials?
* Crossover
* Mutationen


## Parallelisierung

Ebenen

* Berechnungen in neuralen Netzwerken
* gleichzeitige Partien


## Evaluation

* Mit welcher Konfiguration beginnen wir?
    * Größe Feld (eher klein)
    * Größe Netz
    * Anzahl Netze
    * Anzahl Runden
* Wie erkennen wir, dass eine Konfiguration funktioniert bzw. besser ist als
  die vorherige?
