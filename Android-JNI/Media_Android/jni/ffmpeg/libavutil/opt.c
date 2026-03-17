#include "avutil.h"
#include "avstring.h"
#include "opt.h"
#include "eval.h"

//FIXME order them and do a bin search
const AVOption *av_find_opt(void *v, const char *name, const char *unit, int mask, int flags)
{
    AVClass *c= *(AVClass**)v; //FIXME silly way of storing AVClass
    const AVOption *o= c->option;

    for (; o && o->name; o++) {
        if (!strcmp(o->name, name) && (!unit || (o->unit && !strcmp(o->unit, unit))) && (o->flags & mask) == flags)
            return o;
    }
    return NULL;
}

const AVOption *av_next_option(void *obj, const AVOption *last)
{
    if (last && last[1].name) return ++last;
    else if (last)            return NULL;
    else                      return (*(AVClass**)obj)->option;
}

static int av_set_number2(void *obj, const char *name, double num, int den, int64_t intnum, const AVOption **o_out)
{
    const AVOption *o= av_find_opt(obj, name, NULL, 0, 0);
    void *dst;
    if (o_out)
        *o_out= o;
    if (!o || o->offset<=0)
        return AVERROR(ENOENT);

    if (o->max*den < num*intnum || o->min*den > num*intnum) {
        av_log(obj, AV_LOG_ERROR, "Value %lf for parameter '%s' out of range\n", num, name);
        return AVERROR(ERANGE);
    }

    dst= ((uint8_t*)obj) + o->offset;

    switch (o->type) {
    case FF_OPT_TYPE_FLAGS:
    case FF_OPT_TYPE_INT:   *(int       *)dst= llrint(num/den)*intnum; break;
    case FF_OPT_TYPE_INT64: *(int64_t   *)dst= llrint(num/den)*intnum; break;
    case FF_OPT_TYPE_FLOAT: *(float     *)dst= num*intnum/den;         break;
    case FF_OPT_TYPE_DOUBLE:*(double    *)dst= num*intnum/den;         break;
    case FF_OPT_TYPE_RATIONAL:
        if ((int)num == num) *(AVRational*)dst= (AVRational){num*intnum, den};
        else                 *(AVRational*)dst= av_d2q(num*intnum/den, 1<<24);
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}

static const AVOption *av_set_number(void *obj, const char *name, double num, int den, int64_t intnum)
{
    const AVOption *o = NULL;
    if (av_set_number2(obj, name, num, den, intnum, &o) < 0)
        return NULL;
    else
        return o;
}

static const double const_values[] = {
    M_PI,
    M_E,
    FF_QP2LAMBDA,
    0
};

static const char * const const_names[] = {
    "PI",
    "E",
    "QP2LAMBDA",
    0
};

static int hexchar2int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int av_set_string3(void *obj, const char *name, const char *val, int alloc, const AVOption **o_out)
{
    return 0;
}

const AVOption *av_set_double(void *obj, const char *name, double n)
{
    return av_set_number(obj, name, n, 1, 1);
}

const AVOption *av_set_q(void *obj, const char *name, AVRational n)
{
    return av_set_number(obj, name, n.num, n.den, 1);
}

const AVOption *av_set_int(void *obj, const char *name, int64_t n)
{
    return av_set_number(obj, name, 1, 1, n);
}

/**
 *
 * @param buf a buffer which is used for returning non string values as strings, can be NULL
 * @param buf_len allocated length in bytes of buf
 */
const char *av_get_string(void *obj, const char *name, const AVOption **o_out, char *buf, int buf_len)
{
}

static int av_get_number(void *obj, const char *name, const AVOption **o_out, double *num, int *den, int64_t *intnum)
{
    const AVOption *o= av_find_opt(obj, name, NULL, 0, 0);
    void *dst;
    if (!o || o->offset<=0)
        goto error;

    dst= ((uint8_t*)obj) + o->offset;

    if (o_out) *o_out= o;

    switch (o->type) {
    case FF_OPT_TYPE_FLAGS:     *intnum= *(unsigned int*)dst;return 0;
    case FF_OPT_TYPE_INT:       *intnum= *(int    *)dst;return 0;
    case FF_OPT_TYPE_INT64:     *intnum= *(int64_t*)dst;return 0;
    case FF_OPT_TYPE_FLOAT:     *num=    *(float  *)dst;return 0;
    case FF_OPT_TYPE_DOUBLE:    *num=    *(double *)dst;return 0;
    case FF_OPT_TYPE_RATIONAL:  *intnum= ((AVRational*)dst)->num;
                                *den   = ((AVRational*)dst)->den;
                                                        return 0;
    }
error:
    *den=*intnum=0;
    return -1;
}

double av_get_double(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    av_get_number(obj, name, o_out, &num, &den, &intnum);
    return num*intnum/den;
}

AVRational av_get_q(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    av_get_number(obj, name, o_out, &num, &den, &intnum);
    if (num == 1.0 && (int)intnum == intnum)
        return (AVRational){intnum, den};
    else
        return av_d2q(num*intnum/den, 1<<24);
}

int64_t av_get_int(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    av_get_number(obj, name, o_out, &num, &den, &intnum);
    return num*intnum/den;
}

static void opt_list(void *obj, void *av_log_obj, const char *unit,
                     int req_flags, int rej_flags)
{
    const AVOption *opt=NULL;

    while ((opt= av_next_option(obj, opt))) {
        if (!(opt->flags & req_flags) || (opt->flags & rej_flags))
            continue;

        /* Don't print CONST's on level one.
         * Don't print anything but CONST's on level two.
         * Only print items from the requested unit.
         */
        if (!unit && opt->type==FF_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type!=FF_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type==FF_OPT_TYPE_CONST && strcmp(unit, opt->unit))
            continue;
        else if (unit && opt->type == FF_OPT_TYPE_CONST)
            av_log(av_log_obj, AV_LOG_INFO, "   %-15s ", opt->name);
        else
            av_log(av_log_obj, AV_LOG_INFO, "-%-17s ", opt->name);

        switch (opt->type) {
            case FF_OPT_TYPE_FLAGS:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<flags>");
                break;
            case FF_OPT_TYPE_INT:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<int>");
                break;
            case FF_OPT_TYPE_INT64:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<int64>");
                break;
            case FF_OPT_TYPE_DOUBLE:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<double>");
                break;
            case FF_OPT_TYPE_FLOAT:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<float>");
                break;
            case FF_OPT_TYPE_STRING:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<string>");
                break;
            case FF_OPT_TYPE_RATIONAL:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<rational>");
                break;
            case FF_OPT_TYPE_BINARY:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "<binary>");
                break;
            case FF_OPT_TYPE_CONST:
            default:
                av_log(av_log_obj, AV_LOG_INFO, "%-7s ", "");
                break;
        }
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_ENCODING_PARAM) ? 'E' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_DECODING_PARAM) ? 'D' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_VIDEO_PARAM   ) ? 'V' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_AUDIO_PARAM   ) ? 'A' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_SUBTITLE_PARAM) ? 'S' : '.');

        if (opt->help)
            av_log(av_log_obj, AV_LOG_INFO, " %s", opt->help);
        av_log(av_log_obj, AV_LOG_INFO, "\n");
        if (opt->unit && opt->type != FF_OPT_TYPE_CONST) {
            opt_list(obj, av_log_obj, opt->unit, req_flags, rej_flags);
        }
    }
}

int av_opt_show2(void *obj, void *av_log_obj, int req_flags, int rej_flags)
{
    if (!obj)
        return -1;

    av_log(av_log_obj, AV_LOG_INFO, "%s AVOptions:\n", (*(AVClass**)obj)->class_name);

    opt_list(obj, av_log_obj, NULL, req_flags, rej_flags);

    return 0;
}

/** Set the values of the AVCodecContext or AVFormatContext structure.
 * They are set to the defaults specified in the according AVOption options
 * array default_val field.
 *
 * @param s AVCodecContext or AVFormatContext for which the defaults will be set
 */
void av_opt_set_defaults2(void *s, int mask, int flags)
{
    const AVOption *opt = NULL;
    while ((opt = av_next_option(s, opt)) != NULL) {
        if ((opt->flags & mask) != flags)
            continue;
        switch (opt->type) {
            case FF_OPT_TYPE_CONST:
                /* Nothing to be done here */
            break;
            case FF_OPT_TYPE_FLAGS:
            case FF_OPT_TYPE_INT: {
                int val;
                val = opt->default_val;
                av_set_int(s, opt->name, val);
            }
            break;
            case FF_OPT_TYPE_INT64:
                if ((double)(opt->default_val+0.6) == opt->default_val)
                    av_log(s, AV_LOG_DEBUG, "loss of precision in default of %s\n", opt->name);
                av_set_int(s, opt->name, opt->default_val);
            break;
            case FF_OPT_TYPE_DOUBLE:
            case FF_OPT_TYPE_FLOAT: {
                double val;
                val = opt->default_val;
                av_set_double(s, opt->name, val);
            }
            break;
            case FF_OPT_TYPE_RATIONAL: {
                AVRational val;
                val = av_d2q(opt->default_val, INT_MAX);
                av_set_q(s, opt->name, val);
            }
            break;
            case FF_OPT_TYPE_STRING:
            case FF_OPT_TYPE_BINARY:
                /* Cannot set default for string as default_val is of type * double */
            break;
            default:
                av_log(s, AV_LOG_DEBUG, "AVOption type %d of option %s not implemented yet\n", opt->type, opt->name);
        }
    }
}

void av_opt_set_defaults(void *s)
{
    av_opt_set_defaults2(s, 0, 0);
}

/**
 * Store the value in the field in ctx that is named like key.
 * ctx must be an AVClass context, storing is done using AVOptions.
 *
 * @param buf the string to parse, buf will be updated to point at the
 * separator just after the parsed key/value pair
 * @param key_val_sep a 0-terminated list of characters used to
 * separate key from value
 * @param pairs_sep a 0-terminated list of characters used to separate
 * two pairs from each other
 * @return 0 if the key/value pair has been successfully parsed and
 * set, or a negative value corresponding to an AVERROR code in case
 * of error:
 * AVERROR(EINVAL) if the key/value pair cannot be parsed,
 * the error code issued by av_set_string3() if the key/value pair
 * cannot be set
 */
static int parse_key_value_pair(void *ctx, const char **buf,
                                const char *key_val_sep, const char *pairs_sep)
{
    char *key = av_get_token(buf, key_val_sep);
    char *val;
    int ret;

    if (*key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = av_get_token(buf, pairs_sep);
    } else {
        av_log(ctx, AV_LOG_ERROR, "Missing key or no key/value separator found after key '%s'\n", key);
        av_free(key);
        return AVERROR(EINVAL);
    }

    av_log(ctx, AV_LOG_DEBUG, "Setting value '%s' for key '%s'\n", val, key);

    ret = av_set_string3(ctx, key, val, 1, NULL);
    if (ret == AVERROR(ENOENT))
        av_log(ctx, AV_LOG_ERROR, "Key '%s' not found.\n", key);

    av_free(key);
    av_free(val);
    return ret;
}

int av_set_options_string(void *ctx, const char *opts,
                          const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;

    while (*opts) {
        if ((ret = parse_key_value_pair(ctx, &opts, key_val_sep, pairs_sep)) < 0)
            return ret;
        count++;

        if (*opts)
            opts++;
    }

    return count;
}

#ifdef TEST

#undef printf

typedef struct TestContext
{
    const AVClass *class;
    int num;
    int toggle;
    char *string;
    int flags;
    AVRational rational;
} TestContext;

#define OFFSET(x) offsetof(TestContext, x)

#define TEST_FLAG_COOL 01
#define TEST_FLAG_LAME 02
#define TEST_FLAG_MU   04

static const AVOption test_options[]= {
{"num",      "set num",        OFFSET(num),      FF_OPT_TYPE_INT,      0,              0,        100                 },
{"toggle",   "set toggle",     OFFSET(toggle),   FF_OPT_TYPE_INT,      0,              0,        1                   },
{"rational", "set rational",   OFFSET(rational), FF_OPT_TYPE_RATIONAL, 0,              0,        10                  },
{"string",   "set string",     OFFSET(string),   FF_OPT_TYPE_STRING,   0,              CHAR_MIN, CHAR_MAX            },
{"flags",    "set flags",      OFFSET(flags),    FF_OPT_TYPE_FLAGS,    0,              0,        INT_MAX, 0, "flags" },
{"cool",     "set cool flag ", 0,                FF_OPT_TYPE_CONST,    TEST_FLAG_COOL, INT_MIN,  INT_MAX, 0, "flags" },
{"lame",     "set lame flag ", 0,                FF_OPT_TYPE_CONST,    TEST_FLAG_LAME, INT_MIN,  INT_MAX, 0, "flags" },
{"mu",       "set mu flag ",   0,                FF_OPT_TYPE_CONST,    TEST_FLAG_MU,   INT_MIN,  INT_MAX, 0, "flags" },
{NULL},
};

static const char *test_get_name(void *ctx)
{
    return "test";
}

static const AVClass test_class = {
    "TestContext",
    test_get_name,
    test_options
};

int main(void)
{
    int i;

    printf("\nTesting av_set_options_string()\n");
    {
        TestContext test_ctx;
        const char *options[] = {
            "",
            ":",
            "=",
            "foo=:",
            ":=foo",
            "=foo",
            "foo=",
            "foo",
            "foo=val",
            "foo==val",
            "toggle=:",
            "string=:",
            "toggle=1 : foo",
            "toggle=100",
            "toggle==1",
            "flags=+mu-lame : num=42: toggle=0",
            "num=42 : string=blahblah",
            "rational=0 : rational=1/2 : rational=1/-1",
            "rational=-1/0",
        };

        test_ctx.class = &test_class;
        av_opt_set_defaults2(&test_ctx, 0, 0);
        test_ctx.string = av_strdup("default");

        av_log_set_level(AV_LOG_DEBUG);

        for (i=0; i < FF_ARRAY_ELEMS(options); i++) {
            av_log(&test_ctx, AV_LOG_DEBUG, "Setting options string '%s'\n", options[i]);
            if (av_set_options_string(&test_ctx, options[i], "=", ":") < 0)
                av_log(&test_ctx, AV_LOG_ERROR, "Error setting options string: '%s'\n", options[i]);
            printf("\n");
        }
    }

    return 0;
}

#endif
