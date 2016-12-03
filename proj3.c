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
#include <math.h>


/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra NDEBUG,
 * napr.:
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

// vypise ladici informaci o promenne typy int - pouziti DINT(identifikator_promenne)
#define DINT(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti dfloat(identifikator_promenne)
#define DFLOAT(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)
#endif


/*****************************************************************
 * Makra.
 */

#define PRINT_ERR(s) fprintf(stderr, s "\n")
#define PRINTF_ERR(s, ...) fprintf(stderr, s "\n", __VA_ARGS__)


/*****************************************************************
 * Deklarace datovych typu.
 */

struct obj_t {
	int id; // identifikator
	float x; // souradnice x
	float y; // souradnice y
};

struct cluster_t {
	int size; // pocet objektu ve shluku
	int capacity; // kapacita shluku (pocet objektu, pro ktere je rezervovano misto v poli)
	struct obj_t *obj; // ukazatel na pole shluku
};


/*****************************************************************
 * Globalni konstatny.
 */

/// napoveda
const char USAGE_STRING[] =
	"	proj3 SOUBOR [N]\n"
	"		SOUBOR - jmeno souboru se vstupnimi daty\n"
	"		N - volitelny argument definujici cilovy pocet shluk\n";


/// doporucena hodnota pro realokaci shluku
const int CLUSTER_CHUNK = 10;


/// vychozi cilovy pocet shluku
const int DEFAULT_REQUIRED_NUMBER_OF_CLUSTERS = 1;


/*****************************************************************
 * Prototypy funkci.
 */

void sort_cluster(struct cluster_t *c);


/*****************************************************************
 * Deklarace funkci.
 */

/**
 * Pocita pocet cislic v cisle 'n'.
 *
 * @param n cele cislo
 * @return pocet pocet cislic v cisle 'n'
 */
int number_of_digits_in_int(int n)
{
	int count = 0;
	while (abs(n) != 0) {
		n /= 10;
		count++;
	}

	return count;
}


/////////////// Prace se shluky ///////////////

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
		if ((c->obj = malloc(cap * sizeof(struct obj_t)))) {
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


/**
 * Zmena kapacity shluku 'c'.
 *
 * @param c shluk pro zmenu kapacity
 * @param new_cap nova kapacita
 * @return shluk s novou kapacitou, v pripade chyby NULL
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
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
		if ( ! resize_cluster(c, c->capacity + CLUSTER_CHUNK)) {
			return;
		}
	}

	c->obj[c->size++] = obj;
}


/**
 * Do shluku 'c1' prida objekty shluku 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
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


/**
 * Pomocna funkce pro razeni shluku.
 *
 * @param a objekt A
 * @param b objekt B
 * @return -1, 1 nebo 0, pokud je ID objektu A mensi, vetsi nebo rovno ID objektu B
 */
static int obj_sort_compar(const void *a, const void *b)
{
	const struct obj_t *o1 = a;
	const struct obj_t *o2 = b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}


/**
 * Razeni objektu ve shluku vzestupne podle jejich ID.
 *
 * @param c shluk pro serazeni
 */
void sort_cluster(struct cluster_t *c)
{
	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}


/**
 * Hleda objekt podle jeho ID ve shluku.
 *
 * @param cluster shluk, v nemz se vyhladava objekt
 * @param id ID hledaneho objektu
 * @return ukazatel na nalezeny objekt, pokud nebyl nalezeny, vraci NULL
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
 * Tisk shluku na stdout.
 *
 * @param c shluk pro tisk
 */
void print_cluster(struct cluster_t *c)
{
	for (int i = 0; i < c->size; i++) {
		if (i) putchar(' ');
		printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
	}
	putchar('\n');
}


/////////////// Prace s polem shluku ///////////////

/**
 * Inicializace pole shluku, alokuje pamet pro 'size' shluku.
 *
 * @param clusters ukazatel na pole shluku
 * @param size velikost pole shluku
 */
void init_clusters(struct cluster_t **clusters, int size)
{
	assert(clusters);
	assert(size >= 0);

	if ( ! (*clusters = malloc(size * sizeof(struct cluster_t)))) {
		return;
	}

	struct cluster_t cluster;
	for (int i = 0; i < size; i++) {
		init_cluster(&cluster, 0);
		(*clusters)[i] = cluster;
	}
}


/**
 * Odstraneni pole shluku.
 *
 * @param clusters pole shluku pro odstraneni
 * @param size velikost pole shluku
 */
void clear_clusters(struct cluster_t *clusters, int size)
{
	assert(clusters);
	assert(size >= 0);

	for (int i = 0; i < size; i++) {
		clear_cluster(&clusters[i]);
	}
	free(clusters);
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

	clear_cluster(&carr[idx]);
	for (int i = idx; i < narr; i++) {
		carr[i] = carr[i + 1];
	}

	return narr - 1;
}


/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance
 * @param o1 objekt 1
 * @param o2 objekt 2
 * @return Euklidovska vzdalenost mezi objekty 'o1' a 'o2'
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1);
	assert(o2);

	return sqrtf(powf(o1->x - o2->x, 2.0) + powf(o1->y - o2->y, 2.0));
}


/**
 * Pocita vzdalenost dvou shluku.
 *
 * @param c1 shluk 1
 * @param c2 shluk 2
 * @return vzdalenost shluku 'c1' a 'c2'
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1);
	assert(c1->size > 0);
	assert(c2);
	assert(c2->size > 0);

	int i = 0, j = 0;
	float max_distance = obj_distance(&c1->obj[i], &c2->obj[j]), distance;
	for (; i < c1->size; i++) {
		for (int j = i + 1; j < c2->size; j++) {
			if (i == 0 && j == 1) {
				continue;
			}
			distance = obj_distance(&c1->obj[i], &c2->obj[j]);
			if (distance > max_distance) {
				max_distance = distance;
			}
		}
	}

	return max_distance;
}


/**
 * Najde dva nejblizsi shluky.
 * Indexy nalezenych shulu v poli 'carr' uklada do c1 a c2.
 *
 * @param carr pole shluku
 * @param narr pocet shluku v poli
 * @param c1 index jednoho z nalezenych shlku
 * @param c2 index druheho z nalezenych shlku
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	assert(carr);
	assert(narr > 0);

	if (narr == 1) {
		*c1 = *c2 = 0;
		return;
	}

	int i = *c1 = 0, j = *c2 = 1;
	float min_distance = cluster_distance(&carr[i], &carr[j]), distance;
	for (; i < narr; i++) {
		for (j = i + 1; j < narr; j++) {
			if (i == 0 && j == 1) {
				continue;
			}
			distance = cluster_distance(&carr[i], &carr[j]);
			if (distance < min_distance) {
				min_distance = distance;
				*c1 = i;
				*c2 = j;
			}
		}
	}
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
	assert(size >= 0);

	struct obj_t *obj = NULL;
	for (int i = 0; i < size; i++) {
		if ((obj = find_obj_by_id_in_cluster(&clusters[i], id))) {
			break;
		}
	}

	return obj;
}


/**
 * Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'.
 *
 * @param filename nazev souboru pro nacteni objektu
 * @param arr ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
 * @return pocet nactenych shluku (v pripade chyby -1)
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
	assert(arr);

	*arr = NULL;

	// otevreni souboru pro cteni
	FILE *file = fopen(filename, "r");
	if ( ! file) {
		PRINTF_ERR("Nepodarilo se otevrit soubor %s.", filename);
		return -1;
	}

	// alokace pameti pro nacteni radku
	int max_length_of_line = number_of_digits_in_int(INT_MAX) + 11;
	char *line = malloc(max_length_of_line * sizeof(char));
	if ( ! line) {
		PRINT_ERR("Chyba alokace pameti.");
		fclose(file);
		return -1;
	}

	// prochazeni souboru po radcich
	int line_number = 0, number_of_loaded_obects = 0;
	struct obj_t obj;
	while (fgets(line, max_length_of_line, file) && ++line_number) {
		if (line_number == 1) {
			// prvni radek -- nacteni poctu objektu v souboru
			if (sscanf(line, "count=%i", &number_of_loaded_obects) != 1) {
				PRINTF_ERR("Chybny format souboru na radku %i.", line_number);
				free(line);
				fclose(file);
				return -1;
			}

			if (number_of_loaded_obects <= 0) {
				PRINTF_ERR("Hodnota poctu objektu 'count=%i' musi byt vetsi nez 0.", number_of_loaded_obects);
				free(line);
				fclose(file);
				return -1;
			}

			init_clusters(arr, number_of_loaded_obects);
			if ( ! *arr) {
				PRINT_ERR("Chyba alokace pameti.");
				free(line);
				fclose(file);
				return -1;
			}
			continue;
		}

		// pocet radku (-1, protoze na prvnim radku neni objekt) je vetsi, nez je pocet uvedenych objektu
		if (line_number - 1 > number_of_loaded_obects) {
			break;
		}

		// nacteni objektu, validace souradnic, kontrola unikatnosti ID v souboru
		if (
			sscanf(line, "%d %f %f", &obj.id, &obj.x, &obj.y) != 3
			|| obj.x < 0
			|| obj.x > 1000
			|| obj.y < 0
			|| obj.y > 1000
			|| find_obj_by_id_in_array(*arr, number_of_loaded_obects, obj.id)
		) {
			PRINTF_ERR("Chybny format souboru na radku %i.", line_number);
			clear_clusters(*arr, number_of_loaded_obects);
			*arr = NULL;
			free(line);
			fclose(file);
			return -1;
		}

		append_cluster(&(*arr)[line_number - 2], obj);
	}

	free(line);
	fclose(file);

	if (line_number - 1 < number_of_loaded_obects) {
		PRINTF_ERR(
			"Pocet objektu v souboru '%i' musi byt vetsi nebo roven hodnote 'count=%i'",
			line_number - 1,
			number_of_loaded_obects
		);
		clear_clusters(*arr, number_of_loaded_obects);
		*arr = NULL;
		return -1;
	}

	return number_of_loaded_obects;
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
 * Upravi pole shluku na pozadovanou velikost.
 *
 * @param clusters pole shluku
 * @param size velikost pole shluku
 * @param required_size pozadovana velikost pole shluku
 * @return nova velikost pole shluku
 */
int get_required_size_of_clusters(
		struct cluster_t *clusters,
		int size,
		int required_size
) {
	assert(clusters);
	assert(size >= 0);
	assert(required_size > 0);

	if (required_size > size) {
		PRINTF_ERR("Zadany pocet pozadovanych shluku '%d' je vetsi nez celkovy pocet objektu '%d'.", required_size, size);
		return -1;
	}

	int c1_idx, c2_idx;
	while (size > required_size) {
		find_neighbours(clusters, size, &c1_idx, &c2_idx);
		merge_clusters(&clusters[c1_idx], &clusters[c2_idx]);
		size = remove_cluster(clusters, size, c2_idx);
	}

	return size;
}


/////////////// Zpracovani argumentu a main ///////////////

/**
 * Zpracovani vstupnich argumentu.
 *
 * @param argc pocet argumentu programu
 * @param argv argumenty programu
 * @param help bude nastaveno na true, pokud ma byt vypsana napoveda, jinak false
 * @return true, pokud bydy argumenty zpracovany uspesne, jinak false
 */
bool process_input_args(int argc, char *argv[], bool *help)
{
	// 1 povinny argument a 1 volitelny
	if (argc > 1 && argc <= 3) {
		struct cluster_t *clusters;
		int size, required_size = DEFAULT_REQUIRED_NUMBER_OF_CLUSTERS;

		if (argc == 3) {
			char *endptr = NULL;
			required_size = strtol(argv[2], &endptr, 10);
			if (*endptr) {
				PRINTF_ERR("Hodnota argumentu N musi byt cislo, predano %s.", endptr);
				return false;
			} else if (required_size <= 0) {
				PRINTF_ERR("Hodnota argumentu N musi byt cislo vetsi nez 0, predano %d", required_size);
				return false;
			}
		}

		// nacteni shluku
		if ((size = load_clusters(argv[1], &clusters)) == -1) {
			return false;
		}

		if (required_size > size) {
			PRINTF_ERR("Zadany pocet pozadovanych shluku '%d' je vetsi nez celkovy pocet objektu '%d'.", required_size, size);
			// uvolenni pameti
			clear_clusters(clusters, size);
			return false;
		}

		// priprava shluku pro vypis (pozadovany pocet shluku)
		size = get_required_size_of_clusters(clusters, size, required_size);

		// vypis shluku
		print_clusters(clusters, size);
		// uvolenni pameti
		clear_clusters(clusters, size);

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
		printf("napoveda:\n%s\n", USAGE_STRING);
	}
	// navratova hodnota tohoto programu (funkce main)
	// zavisi na navratove hodnote funkce process_input_args
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
