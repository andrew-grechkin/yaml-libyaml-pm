/* These definitions affect -pedantic warnings...

#define PERL_GCC_BRACE_GROUPS_FORBIDDEN 1
#define __STRICT_ANSI__ 1
#define PERL_GCC_PEDANTIC 1
*/

#include "EXTERN.h"
#include "perl.h"
#define NO_XSLOCKS
#include "XSUB.h"
#define NEED_newRV_noinc
#define NEED_sv_2pv_nolen
#define NEED_sv_2pvbyte
#include "ppport.h"
#include <yaml.h>
#include <ppport_sort.h>

#define TAG_PERL_PREFIX "tag:yaml.org,2002:perl/"
#define TAG_PERL_REF TAG_PERL_PREFIX "ref"
#define TAG_PERL_STR TAG_PERL_PREFIX "str"
#define TAG_PERL_GLOB TAG_PERL_PREFIX "glob"
#define ERRMSG "YAML::XS Error: "
#define LOADERRMSG "YAML::XS::Load Error: "
#define DUMPERRMSG "YAML::XS::Dump Error: "

typedef enum yaml_xs_scalar_type_e {
    YAML_XS_SCALAR_TYPE_STRING,
    YAML_XS_SCALAR_TYPE_BOOL_TRUE,
    YAML_XS_SCALAR_TYPE_BOOL_FALSE,
    YAML_XS_SCALAR_TYPE_NULL,
    YAML_XS_SCALAR_TYPE_FLOAT_INF,
    YAML_XS_SCALAR_TYPE_FLOAT_NAN,
    YAML_XS_SCALAR_TYPE_INT_OCT,
    YAML_XS_SCALAR_TYPE_INT_HEX,
    YAML_XS_SCALAR_TYPE_INT,
    YAML_XS_SCALAR_TYPE_FLOAT,
} yaml_xs_scalar_type_t;

typedef struct {
    yaml_parser_t parser;
    yaml_event_t event;
    HV *anchors;
    int load_code;
    int load_bool_jsonpp;
    int load_bool_boolean;
    int load_blessed;
    int forbid_duplicate_keys;
    int document;
} perl_yaml_loader_t;

typedef struct {
    yaml_emitter_t emitter;
    long anchor;
    HV *anchors;
    HV *shadows;
    int dump_code;
    int dump_bool_jsonpp;
    int dump_bool_boolean;
    int quote_number_strings;
} perl_yaml_dumper_t;

typedef struct {
    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_event_t event;
    long anchor;
    HV *anchors;
    int indent;
    int force_sequence_indent;
    int utf8;
    int header;
    int footer;
    int width;
    int require_footer;
    char *anchor_prefix;
    int document;
} perl_yaml_xs_t;

static SV *
call_coderef(SV *, AV *);

static SV *
fold_results(I32);

static SV *
find_coderef(char *);

void
set_dumper_options(perl_yaml_dumper_t *);

void
set_loader_options(perl_yaml_dumper_t *);

void
Dump(SV *, ...);

void
Load(SV *);

SV *
load_node(perl_yaml_loader_t *);

SV *
load_mapping(perl_yaml_loader_t *, char *);

SV *
load_sequence(perl_yaml_loader_t *);

SV *
load_scalar(perl_yaml_loader_t *);

SV *
load_alias(perl_yaml_loader_t *);

SV *
load_scalar_ref(perl_yaml_loader_t *);

SV *
load_code(perl_yaml_loader_t *);

SV *
load_regexp(perl_yaml_loader_t *);

SV *
load_glob(perl_yaml_loader_t *);


void
dump_prewalk(perl_yaml_dumper_t *, SV *);

void
dump_document(perl_yaml_dumper_t *, SV *);

void
dump_node(perl_yaml_dumper_t *, SV *);

void
dump_hash(perl_yaml_dumper_t *, SV *, yaml_char_t *, yaml_char_t *);

void
dump_array(perl_yaml_dumper_t *, SV *);

void
dump_scalar(perl_yaml_dumper_t *, SV *, yaml_char_t *);

void
dump_ref(perl_yaml_dumper_t *, SV *);

void
dump_code(perl_yaml_dumper_t *, SV *);

SV*
dump_glob(perl_yaml_dumper_t *, SV *);


yaml_char_t *
get_yaml_anchor(perl_yaml_dumper_t *, SV *);

yaml_char_t *
get_yaml_tag(SV *);


int
append_output(void *, unsigned char *, size_t size);


void
oo_load_stream(perl_yaml_xs_t *);
SV *
oo_load_node(perl_yaml_xs_t *);
SV *
oo_load_sequence(perl_yaml_xs_t *);
SV *
oo_load_mapping(perl_yaml_xs_t *);
SV *
oo_load_scalar(perl_yaml_xs_t *);
SV *
oo_load_alias(perl_yaml_xs_t *);

void
oo_dump_stream(perl_yaml_xs_t *, ...);
void
oo_dump_document(perl_yaml_xs_t *, SV *node);
void
oo_dump_node(perl_yaml_xs_t *, SV *node);
void
oo_dump_hash(perl_yaml_xs_t *, SV *node, yaml_char_t *);
void
oo_dump_array(perl_yaml_xs_t *, SV *node, yaml_char_t *);
void
oo_dump_scalar(perl_yaml_xs_t *, SV *node);
void
oo_dump_prewalk(perl_yaml_xs_t *, SV *);
yaml_char_t *
oo_get_yaml_anchor(perl_yaml_xs_t *, SV *);

