{% for param, name, typ, namehash, minvalue, maxvalue, defvalue, button in jdata %}
// {{param}} {{name}} {{typ}} {{namehash}}
#define HV_NAME_CHANNEL_{{param}} "{{name}}"
#define HV_HASH_{{typ}}_CHANNEL_{{param}} {{namehash}}
#define HV_MIN_CHANNEL_{{param}} {{minvalue}}
#define HV_MAX_CHANNEL_{{param}} {{maxvalue}}
{% if defvalue != None %}
#define HV_DEFAULT_CHANNEL_{{param}} {{(defvalue-minvalue)/(maxvalue-minvalue)}}
{% endif %}
{% endfor %}
