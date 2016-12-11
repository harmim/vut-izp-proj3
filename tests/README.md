## Automatizovany test 3. projektu pro predmet IZP

## autor: v.obrusnik@gmail.com

### Spusteni
Pokud se nejedna o uklid, kazde spusteni testu vyvola preklad zdrojoveho
kodu projektu a spusteni zakladnich testu.

Spusteni testu:
	bash ./test.sh

Spusteni testu a overeni prace s pameti:
	bash ./test.sh -m

Uklid souboru vytvorenych v prubehu testovani:
	bash ./test.sh -c

### Obsah souboru
Vstup testu je pripraven v souboru:
	test{cislo testu}-in

Ocekavany vystup testu je pripraven v souboru:
	test{cislo testu}-out

Vystup programu je ulozen do souboru:
	test{cislo testu}-my

Chybovy vystup programu je ulozen do souboru:
	test{cislo testu}-error

#### Test 1
Na ukazkovych datech z wiki testuje nacteni 20 clusteru a vypsani 20 clusteru
v nezmenene podobe.
Vystup je porovnan proti ukazkovemu vystupu z wiki.

#### Test 2
Na ukazkovych datech z wiki testuje nacteni 20 clusteru a vypsani 8 clusteru
modifikovanych podle implementovaneho algoritmu.
Vystup je porovnan proti ukazkovemu vystupu z wiki.

#### Test 3
Na ukazkovych datech z wiki testuje nacteni 20 clusteru a vypsani 8 clusteru
modifikovanych podle implementovaneho algoritmu.
Vystup je porovnan proti ukazkovemu vystupu z wiki.

#### Test 4
Zpracovani parametru. N neni cislo.
Nevyzaduje vstupni soubor.

#### Test 5
Zpracovani parametru. Vice nez 3 parametry.
Nevyzaduje vstupni soubor.

#### Test 6
Zpracovani parametru. Chybejici parametry.
Nevyzaduje vstupni soubor.

#### Test 7
Test otevreni neexistujiciho vstupniho souboru.
Nevyzaduje vstupni soubor.

#### Test 8
Test dosazeni vychozi hodnoty 1

#### Test 9
Format vstupniho souboru. Count je vetsi nez pocet definovanych objektu.

#### Test 10
Format vstupniho souboru. Count je mensi nez pocet definovanych objektu.

#### Test 11
Format vstupniho souboru. Vice objektu na radku.

#### Test 12
Format vstupniho souboru. Nahodny textovy soubor.

#### Test 13
Format vstupniho souboru. OBJID neni cislo.

#### Test 14
Format vstupniho souboru. Souradnice X neni cislo.

#### Test 15
Format vstupniho souboru. Souradnice Y neni cislo.

#### Test 16
Format vstupniho souboru. Souradnice X a Y mimo interval (x,y < 0).

#### Test 17
Format vstupniho souboru. Souradnice X a Y mimo interval (x,y > 1000).

#### Test 18
Format vstupniho souboru. OBJID neni cele cislo.

#### Test 19
Format vstupniho souboru. Souradnice X a Y nejsou cela cisla.

#### Test 20
Format vstupniho souboru. Duplicitni pouziti ID.

#### Test 21
Format vstupniho souboru. ID > INT_MAX.

--- 
### Testy pri parametru -m

Ty same a kontroluje se jen Valgrindem kontrola prace s pameti.
