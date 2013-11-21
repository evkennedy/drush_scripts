<?php

/**
 * @file
 *  Parent script file for Islandora object management functions. All commands are described here along with
 *  common use functions. Specific custom functions will need to be included in an .inc file and referenced here.
 *  Put this file wherever you put your Drush commands.
 *  Example: sites/all/drush/
 */

/**
 * Implementation of hook_drush_command().
 *
 * @return array
 *   An associative array describing your command(s).
 */
function islandora_object_management_drush_command() {

  $items = array();

  $items['islandora-object-management'] = array(
    'description' => '',
    'arguments' => array(
      'file' => '',
    ),
    'examples' => array(
      'drush -u 1 objmanage',
    ),
    'aliases' => array('objmanage'),
    // We can pass in users id on the command line using drush -u.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

function drush_islandora_object_management() {

  drush_print("\n\n+-------------------------+\n  Object Management  \n+-------------------------+");

  $objmanage_menu[1] = "";
  $objmanage_menu[2] = "";
  $objmanage_menu[3] = "";

  drush_print("\n");
}