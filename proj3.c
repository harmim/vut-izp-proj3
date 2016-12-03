/**
 * @name Projekt 3 - Jednoducha shlukova analyza (Complete linkage)
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @see http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
//#include <math.h> // sqrtf

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define DEBUG(s)
#define DFMT(s, ...)
#define DINT(i)
#define DFLOAT(f)
#else
// vypise ladici retezec
#define DEBUG(s) printf(" - %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define DFMT(s, ...) printf(" - " __FILE__ ":%u: " s "\n", __LINE__, __VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define DINT(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define DFLOAT(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)
#endif

#define PRINT_ERR(s) fprintf(stderr, s "\n")
#define PRINTF_ERR(s, ...) fprintf(stderr, s "\n", __VA_ARGS__)

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
	int id;
	float x;
	float y;
};

struct cluster_t {
	int size;
	int capacity;
	struct obj_t *obj;
};

const char usage_string[] =
	"		proj3 SOUBOR [N]\n"
	"			SOUBOR - jmeno souboru se vstupnimi daty\n"
	"			N - volitelny argument definujici cilovy pocet shluk\n";


/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/**
 * Inicializace shluku 'c', alokuje pamet pro 'cap' objektu.
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @param c shluk pro inicializaci
 * @param cap kapacita objektu
 */
void init_cluster(struct cluster_t *c, int cap)
{
	assert(c);
	assert(cap >= 0);

	c->size = 0;
	if (cap > 0) {
		struct obj_t *obj_arr = malloc(cap * sizeof(struct obj_t));
		if (obj_arr != NULL) {
			c->obj = obj_arr;
			c->capacity = cap;
			return;
		}
	}
	c->capacity = 0;
	c->obj = NULL;
}


/**
 * Odstraneni vsech objektu shluku 'c' a inicializace na prazdny shluk.
 *
 * @param c shluk pro odstraneni
 */
void clear_cluster(struct cluster_t *c)
{
	assert(c);
	free(c->obj);
	init_cluster(c, 0);
}


/// doporucena hodnota pro realokaci shluku
const int CLUSTER_CHUNK = 10;


/**
 * Zmena kapacity shluku 'c'.
 *
 * @param c shluk pro zmenu kapacity
 * @param new_cap nova kapacita
 * @return shluk s novou kapacitou, v pripade chyby NULL
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
	// TUTO FUNKCI NEMENTE
	assert(c);
	assert(c->capacity >= 0);
	assert(new_cap >= 0);

	if (c->capacity >= new_cap)
		return c;

	size_t size = sizeof(struct obj_t) * new_cap;

	void *arr = realloc(c->obj, size);
	if (arr == NULL)
		return NULL;

	c->obj = arr;
	c->capacity = new_cap;
	return c;
}


/**
 * Prida objekt 'obj' na konec shluku 'c',
 * pokud se do shluku objekt nevejde, rozsiri shluk.
 *
 * @param c shluk pro pridani objektu
 * @param obj objekt, ktery bude pridan do shluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	assert(c);

	if (c->capacity <= c->size) {
		if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL) {
			return;
		}
	}

	c->obj[c->size++] = obj;
}


/**
 * Seradi objekty ve shluku 'c' vzestupne podle jejich ID.
 *
 * @param c shluk pro serazeni
 */
void sort_cluster(struct cluster_t *c);


/**
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazny vzestupne podle ID. Shluk 'c2' bude nezmenen.
 *
 * @param c1 shluk 1
 * @param c2 shluk 2
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1);
	assert(c2);

	for (int i = 0; i < c2->size; i++) {
		append_cluster(c1, c2->obj[i]);
	}

	sort_cluster(c1);
}


/**********************************************************************/
/* Prace s polem shluku */


/**
 * Inicializace pole shluku, alokuje pamet pro 'size' shluku.
 *
 * @param arr ukazatel na pole shluku
 * @param size velikost pole shluku
 */
void init_clusters(struct cluster_t **arr, int size)
{
	assert(arr);
	assert(size >= 0);

	if ((*arr = malloc(size * sizeof(struct cluster_t))) == NULL) {
		return;
	}

	struct cluster_t cluster;
	for (int i = 0; i < size; i++) {
		init_cluster(&cluster, 0);
		(*arr)[i] = cluster;
	}
}


/**
 * Odstraneni pole shluku.
 *
 * @param arr pole shluku pro odstraneni
 * @param size velikost pole shluku
 */
void clear_clusters(struct cluster_t **arr, int size)
{
	assert(arr);
	assert(size >= 0);

	for (int i = 0; i < size; i++) {
		clear_cluster(&(*arr)[i]);
	}
	free(*arr);
}


/**
 * Odstrani shluk z pole shluku 'carr'.
 *
 * @param carr pole shluku
 * @param narr pocet shluku v poli
 * @param idx index shluku pro odstraneni
 * @return novy pocet shluku v poli
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(idx < narr);
	assert(narr > 0);
	assert(idx >= 0);

	// alokace noveho pole o mensi velikosti
	struct cluster_t *new_carr = malloc((narr - 1) * sizeof(struct obj_t));
	if (new_carr == NULL) {
		return narr;
	}

	// presunuti vsech prvku, korome prvku, ktery ma byt smazan do noveho pole
	for (int i = 0, j = 0; i < narr; i++) {
		if (i != idx) {
			new_carr[j++] = carr[i];
		}
	}
	// smazani prvku
	clear_cluster(&carr[idx]);

	return narr - 1;
}


/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1 != NULL);
	assert(o2 != NULL);

	// TODO
	return 0.0;
}


/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
	assert(c1->size > 0);
	assert(c2 != NULL);
	assert(c2->size > 0);

	// TODO
	return 0.0;
}


/**
 * Najde dva nejblizsi shluky.
 * Indexy nalezenych shulu v poli carr uklada do c1 a c2.
 *
 * @param carr pole shluku
 * @param narr pocet shluku v poli
 * @param c1 index jednoho z nalezenych shlku
 * @param c2 index druheho z nalezenych shlku
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	(void) *carr;
	(void) *c1;
	(void) *c2;
	assert(narr > 0);

	// TODO
}


/**
 * Pomocna funkce pro razeni shluku.
 *
 * @param a objekt A
 * @param b objekt B
 * @return -1, 1 nebo 0, pokud je ID objektu A mensi, vetsi nebo rovno ID objektu B
 */
static int obj_sort_compar(const void *a, const void *b)
{
	// TUTO FUNKCI NEMENTE
	const struct obj_t *o1 = a;
	const struct obj_t *o2 = b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}


/**
 * Razeni objektu ve shluku vzestupne podle jejich ID.
 * @param c shluk pro serazeni
 */
void sort_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}


/**
 * Hleda objekt podle jeho ID ve shluku.
 *
 * @param cluster shluk, v nemz se vyhladava objekt
 * @param id ID hledaneho objektu
 * @return nalezeny objekt, pokud nebyl nalezeny, vraci NULL
 */
struct obj_t *find_obj_by_id_in_cluster(struct cluster_t *cluster, int id)
{
	assert(cluster);

	for (int i = 0; i < cluster->size; i++) {
		if (cluster->obj[i].id == id) {
			return &cluster->obj[i];
		}
	}

	return NULL;
}


/**
 * Hleda objekt podle jeho ID v poli shluku.
 *
 * @param clusters pole shluku, v nemz se vyhladava objekt
 * @param size velikost pole shluku
 * @param id ID hledaneho objektu
 * @return nalezeny objekt, pokud nebyl nalezeny, vraci NULL
 */
struct obj_t *find_obj_by_id_in_array(struct cluster_t *clusters, int size, int id)
{
	assert(clusters);
	assert(size > 0);

	struct obj_t *obj = NULL;
	for (int i = 0; i < size; i++) {
		if ((obj = find_obj_by_id_in_cluster(&clusters[i], id))) {
			break;
		}
	}

	return obj;
}


/**
 * Tisk shluku na stdout.
 *
 * @param c shluk pro tisk
 */
void print_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	for (int i = 0; i < c->size; i++) {
		if (i) putchar(' ');
		printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
	}
	putchar('\n');
}


/**
 * Pocita pocet cislic v cisle 'n'.
 *
 * @param n cele cislo
 * @return pocet pocet cislic v cisle 'n'
 */
int number_of_digits_in_int(int n)
{
	int count = 0;

	while (n != 0) {
		n /= 10;
		count++;
	}

	return count;
}


/**
 * Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'.
 *
 * @param filename nazev souboru pro nacteni objektu
 * @param arr ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
 * @return pocet nastenych shluku (v pripade chyby -1)
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
	assert(arr);
	*arr = NULL;

	// otevreni souboru pro cteni
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		PRINTF_ERR("Nepodarilo se otevrit soubor %s.", filename);
		return -1;
	}

	// alokace pameti pro nacteni radku
	int max_length_of_line = number_of_digits_in_int(INT_MAX) + 11;
	char *line = malloc(max_length_of_line * sizeof(char));
	if (line == NULL) {
		PRINT_ERR("Chyba alokace pameti.");
		fclose(file);
		return -1;
	}

	// prochazeni souboru po radcich
	int line_number = 0, number_of_loaded_cluesters = 0, obj_id, x, y;
	struct obj_t obj;
	while (fgets(line, max_length_of_line, file) && ++line_number) {
		if (line_number == 1) {
			// prvni radek -- nacteni poctu objektu v souboru
			if (sscanf(line, "count=%i", &number_of_loaded_cluesters) != 1) {
				PRINTF_ERR("Chybny format souboru na radku %i.", line_number);
				free(line);
				fclose(file);
				return -1;
			}

//			*arr = malloc(number_of_loaded_cluesters * sizeof(struct cluster_t));

//			struct cluster_t cluster;
//			for (int i = 0; i < number_of_loaded_cluesters; i++) {
//				init_cluster(&cluster, 1);
//				(*arr)[i] = cluster;
//			}

			init_clusters(arr, number_of_loaded_cluesters);
			if (*arr == NULL) {
				PRINT_ERR("Chyba alokace pameti.");
				free(line);
				fclose(file);
				return -1;
			}
			continue;
		}

		// pocet radku (-1, protoze na prvnim radku neni objekt) je vetsi, nez je pocet uvedenych objektu
		if (line_number - 1 > number_of_loaded_cluesters) {
			break;
		}

		// nacteni objektu, validace souradnic, kontrola unikatnosti ID v souboru
		if (
			sscanf(line, "%i %i %i", &obj_id, &x, &y) != 3
			|| x < 0
			|| x > 1000
			|| y < 0
			|| y > 1000
			|| find_obj_by_id_in_array(*arr, number_of_loaded_cluesters, obj_id)
		) {
			PRINTF_ERR("Chybny format souboru na radku %i.", line_number);
			free(line);
			fclose(file);
			return -1;
		}

		obj.id = obj_id;
		obj.x = x;
		obj.y = y;
		append_cluster(&(*arr)[line_number - 2], obj);
	}

	free(line);
	fclose(file);

	return number_of_loaded_cluesters;
}


/**
 * Tisk pole shluku na stdout.
 *
 * @param carr pole shluku pro tisk
 * @param narr pocet shluku v poli
 */
void print_clusters(struct cluster_t *carr, int narr)
{
	printf("Clusters:\n");
	for (int i = 0; i < narr; i++) {
		printf("cluster %d: ", i);
		print_cluster(&carr[i]);
	}
}


/**
 * Zpracovani vstupnich argumentu.
 *
 * @param argc pocet argumentu programu
 * @param argv argumenty programu
 * @param help true, pokud ma byt vypsana napoveda, jinak false
 * @return true, pokud bydy argumenty zpracovany uspesne, jinak false
 */
bool process_input_args(int argc, char *argv[], bool *help)
{
	// 1 povinny argument a 1 volitelny
	if (argc > 1 && argc <= 3) {
		struct cluster_t *clusters;
		int number_of_loaded_cluesters;

		if ((number_of_loaded_cluesters = load_clusters(argv[1], &clusters)) == -1) {
			return false;
		}

//		if (argc == 3) {
//			 TODO
//		}

		print_clusters(clusters, number_of_loaded_cluesters);
		clear_clusters(&clusters, number_of_loaded_cluesters);

		return true;
	}

	// nezname argumenty
	*help = true;
	return true;
}


/**
 * @param argc pocet argumentu programu
 * @param argv argumenty programu
 * @retun navratova hodnota programu -> 0, pokud program skoncil uspesne, jinak 1
 */
int main(int argc, char *argv[])
{
	bool help = false, result = process_input_args(argc, argv, &help);
	if (help) {
		// bude vypsana napoveda
		printf("napoveda:\n%s\n", usage_string);
	}
	// navratova hodnota tohoto programu (funkce main)
	// zavisi na navratove hodnote funkce process_input_args
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
