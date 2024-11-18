import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.const as ehc
from esphome.core import coroutine_with_priority

from . import const as c
from .receive_trigger import validate_receive_trigger, receive_trigger_to_code

esp_now_ns = cg.esphome_ns.namespace('esp_now')
ESPNow = esp_now_ns.class_('ESPNow', cg.Component)

CODEOWNERS = ["@medisoft"]

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ESPNow),
            cv.Optional(ehc.CONF_CHANNEL): cv.All(cv.int_, cv.Range(min=1, max=14)),
            cv.Optional(ehc.CONF_PASSWORD): cv.All(cv.string, cv.Length(min=2)),
            cv.Optional(c.CONF_ON_RECEIVE): validate_receive_trigger,
        }
    ),
)



# CONFIG_SCHEMA = cv.Schema({
#     cv.GenerateID(): cv.declare_id(ESPNow),
#     cv.Required(c.CONF_CHANNEL): cv.int_range(min=1, max=14),  # Canal de Wi-Fi, rango de 1 a 13
#     cv.Required(c.CONF_PASSWORD): cv.All(cv.string, cv.Length(min=2)),
#     # cv.Optional(c.CONF_ON_RECEIVE): validate_receive_trigger,
# }).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[ehc.CONF_ID])
    yield cg.register_component(var, config)
