import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import gree_ns
from ..climate import GreeClimate

GreeHorizontalSwingSelect = gree_ns.class_("GreeHorizontalSwingSelect", select.Select)
GreeVerticalSwingSelect = gree_ns.class_("GreeVerticalSwingSelect", select.Select)

HORIZONTAL_SWING = "horizontal_swing"
VERTICAL_SWING = "vertical_swing"

CONF_GREE_ID = "gree_id"

CONFIG_SCHEMA = {
    cv.Required(CONF_GREE_ID): cv.use_id(GreeClimate),
    cv.Optional(HORIZONTAL_SWING): select.select_schema(
        GreeHorizontalSwingSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:arrow-left-right",
    ),
    cv.Optional(VERTICAL_SWING): select.select_schema(
        GreeVerticalSwingSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:arrow-up-down",
    ),
}


async def to_code(config):
    parent = await cg.get_variable(config[CONF_GREE_ID])

    if horizontal_swing_config := config.get(HORIZONTAL_SWING):
        s = await select.new_select(
            horizontal_swing_config,
            options=[
                "Off",
                "Auto",
                "Left",
                "Middle left",
                "Middle",
                "Middle right",
                "Right",
            ],
        )
        await cg.register_parented(s, parent)

    if vertical_swing_config := config.get(VERTICAL_SWING):
        s = await select.new_select(
            vertical_swing_config,
            options=["Off", "Auto", "Up", "Middle up", "Middle", "Middle down", "Down"],
        )
        await cg.register_parented(s, parent)
