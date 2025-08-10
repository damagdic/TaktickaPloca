# TaktickaPloca

Ova aplikacija predstavlja taktičku ploču za nogomet u kojoj korisnik može:
- Pogled na igralište sa 22 igrača
- Pogled na igralište sa 11 igrača -> trening opcija
- Pomjerati igrače bijelog i crvenog tima po terenu
- Pomjerati loptu po terenu
- Crtanje linija (dodavanja) između igrača
- Automatski izlazak iz načina crtanja linija
- Crtati pomoću GDI funkcija uz podršku za double buffering (bez treperenja)
- Save/Load taktika

# Funkcionalnosti

Nogometni teren:
- Zelena pozadina
- Bijele linije terena (okvir, centar, šesnaesterac, peterac, krug, polukrugovi)
- Prilagodljiv prozoru (responsive)

Igrači:
- Bijela (s golmanom u plavom)
- Crvena (s golmanom u crnom)
- Mogu se klikom i povlačenjem pomjerati po terenu

Lopta:
- Crna kružnica
- Može se pomjerati po istom principu kao i igrači

Crtanje linija:
- U meniju "Edit" može se uključiti "Add Line Mode"
- Klikom na početnu točku, pa na završnu, crta se crna linija sa strelicom
- Automatski izlazak iz načina crtanja:
   nakon nacrtane linije ili ako se klikne na igrača/loptu

Pogledi: 
- Full Field — cijeli teren
- Half Field — lijeva polovica, zumirana
- Promjenom pogleda linije i pozicije se resetiraju

Ostalo:
- Double buffering (pomoću `CreateCompatibleDC`, `BitBlt`) za glatko crtanje
- Skalabilni teren — prilagođava se veličini prozora
- Crtanje pomoću GDI funkcija (Ellipse, Rectangle, LineTo, DrawText)
- Spremanje fileova kao .txt te mogućnost otvaranja istih

# Kako pokrenuti projekt

1. Otvoriti `.sln` projekt u Visual Studiu
2. Provjeriti koristi li se **C++20** u Debug i Release konfiguraciji:
   - Project → Properties → C/C++ → Language → `C++20`
3. Build & Run (`Ctrl+F5`)
