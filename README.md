# TaktickaPloca

Ova aplikacija predstavlja taktičku ploču za nogomet u kojoj korisnik može:
- Pogled na igralište sa 22 igrača
- Pomjerati igrače bijelog i crvenog tima po terenu
- Pomjerati loptu po terenu
- Sve se crta pomoću GDI funkcija uz podršku za double buffering (bez treperenja)

# Funkcionalnosti

Nogometni teren:
- Zelena pozadina
- Bijele linije terena (okvir, centar, šesnaesterac, peterac, krug, polukrugovi)

Igrači:
- Dvije ekipe (bijela i crvena)
- Mogu se klikom i povlačenjem pomjerati po terenu

Lopta:
- Crna kružnica
- Može se pomjerati po istom principu kao i igrači

Ostalo:
- Double buffering (pomoću `CreateCompatibleDC`, `BitBlt`) za glatko crtanje
- Skalabilni teren — prilagođava se veličini prozora

# Kako pokrenuti projekt

1. Otvori `.sln` projekt u Visual Studiu
2. Provjeri da koristiš **C++20** u Debug i Release konfiguraciji:
   - Project → Properties → C/C++ → Language → `C++20`
3. Build & Run (`Ctrl+F5`)
