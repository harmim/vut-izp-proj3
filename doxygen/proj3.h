/**
 * @mainpage Projekt 3
 * @link
 * proj3.h
 * @endlink
 *
 * @file proj3.h
 * @brief Projekt 3 - Jednoducha shlukova analyza (Complete linkage)
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date Prosinec 2016
 * @version 1.0
 * @see http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */


/**
 * @brief Struktura reprezentujici objekt s urcitymi souradnicemi.
 */
struct obj_t {
	/** identifikator */
	int id;
	/** souradnice x */
	float x;
	/** souradnice y */
	float y;
};

/**
 * @brief Struktura reprezentujici shluk objektu.
 */
struct cluster_t {
	/** pocet objektu ve shluku */
	int size;
	/** kapacita shluku (pocet objektu, pro ktere je rezervovano misto v poli) */
	int capacity;
	/** pole objektu nalezici danemu shluku */
	struct obj_t *obj;
};


/**
 * @defgroup clusters Prace se shluky
 * @{
 */

/**
 * Inicializace shluku `c`, alokuje pamet pro `cap` objektu.
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @post
 * Shluk `c` bude mit alokovanou pamet pro `cap` objketu,
 * pokud nenastane chyba pri alokaci.
 *
 * @param c shluk pro inicializaci
 * @param cap pozadovana kapacita shluku
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Odstraneni vsech objektu shluku `c` a inicializace na prazdny shluk.
 *
 * @post
 * Alokovana pamet pro vsechny objekty shluku `c` bude uvolnena.
 *
 * @param c shluk pro odstraneni
 */
void clear_cluster(struct cluster_t *c);

/// hodnota pro realokaci shluku
extern const int CLUSTER_CHUNK;

/**
 * Zmena kapacity shluku `c`.
 *
 * @pre
 * Kapacita shluku `c` bude vetsi nebo rovna 0.
 *
 * @post
 * Kapacita shluku `c` bude zmenena na novou kapacitu `new_cap`,
 * pokud nenastane chyba pri alokaci.
 *
 * @param c shluk pro zmenu kapacity
 * @param new_cap nova kapacita
 * @return shluk s novou kapacitou, v pripade chyby NULL
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Prida objekt `obj` na konec shluku `c`,
 * pokud se do shluku objekt nevejde, rozsiri shluk.
 *
 * @pre
 * Pocet objektu ve shluku bude vetsi nebo roven 0.
 *
 * @post
 * Na posledni pozici shluku `c` bude objket `obj`,
 * pokud nenastane chyba pri alokaci.
 *
 * @param c shluk pro pridani objektu
 * @param obj objekt, ktery bude pridan do shluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Do shluku `c1` prida objekty shluku `c2`. Shluk `c1` bude v pripade nutnosti rozsiren.
 * Objekty ve shluku `c1` budou serazny vzestupne podle ID. Shluk `c2` bude nezmenen.
 *
 * @pre
 * Pocet objektu ve shluku `c2` bude vetsi nebo roven 0.
 *
 * @post
 * Shluk `c1` bude rozsiren o objekty shluku `c2`, pokud nenastane chyba
 * pri alokaci.
 *
 * @post
 * Objekty ve shluku `c1` budou serazeny vzestupne podle ID.
 *
 * @param c1 shluk, do ktereho budou pridany objekty shluku `c2`
 * @param c2 shluk, jehoz objekty budou pridany do shluku `c1`
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Razeni objektu ve shluku vzestupne podle jejich ID.
 *
 * @post
 * Objekty ve shluku `c` budou serazeny vzestupne podle ID.
 *
 * @param c shluk pro serazeni
 */
void sort_cluster(struct cluster_t *c);

/**
 * Tisk shluku na stdout.
 *
 * @post
 * Objekty shluku `c` budou vypsany na stdout.
 *
 * @param c shluk pro tisk
 */
void print_cluster(struct cluster_t *c);

/**
 * @}
 */


/**
 * @defgroup array_of_clusters Prace s polem shluku
 * @{
 */

/**
 * Odstrani shluk z pole shluku `carr`.
 *
 * @post
 * Z pole shluku `carr` bude odstranen prvek na indexu `idx`
 * a pole bude o 1 mensi.
 *
 * @param carr pole shluku
 * @param narr pocet shluku v poli
 * @param idx index shluku pro odstraneni
 * @return novy pocet shluku v poli
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance
 *
 * @param o1 objekt 1
 * @param o2 objekt 2
 * @return Euklidovska vzdalenost mezi objekty `o1` a `o2`
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Pocita vzdalenost dvou shluku.
 *
 * @pre
 * Pocet objektu ve shluku `c1` bude vetsi nez 0.
 *
 * @pre
 * Pocet objektu ve shluku `c2` bude vetsi nez 0.
 *
 * @param c1 shluk 1
 * @param c2 shluk 2
 * @return vzdalenost shluku `c1` a `c2`
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Najde dva nejblizsi shluky.
 * Indexy nalezenych shulu v poli `carr` uklada do `c1` a `c2`.
 *
 * @post
 * Indexy dvou nejblizsich shluku budou ulozeny v `c1` a `c2`.
 *
 * @param carr pole shluku
 * @param narr pocet shluku v poli
 * @param c1 index jednoho z nalezenych shlku
 * @param c2 index druheho z nalezenych shlku
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Ze souboru `filename` nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr `arr`.
 *
 * @pre
 * Bude existovat soubor `filename` a program bude mit prava pro jeho cteni.
 *
 * @pre
 * Data v souboru budou ve spravnem formatu.
 *
 * @post
 * Pro kazdy objekt uvedeny ve vstupnim souboru bude vytvoren shluk,
 * vsechny tyto shluky budou ulozeny v poli shluku `arr`, ktere bude
 * mit alokovanou pamet pro pocet shluku uvedeny ve vstupnim souboru,
 * pokud nenastane chyba pri alokaci.
 *
 * @param filename nazev souboru pro nacteni objektu
 * @param arr ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
 * @return pocet nactenych shluku (v pripade chyby -1)
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Tisk pole shluku na stdout.
 *
 * @post
 * Objekty vsech shluku v poli shluku `carr` budou vypsany na stdout.
 *
 * @param carr pole shluku pro tisk
 * @param narr pocet shluku v poli
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * @}
 */
