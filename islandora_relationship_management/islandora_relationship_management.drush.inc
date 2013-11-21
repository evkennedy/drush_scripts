<?php

/**
 * @file
 *  Parent script file for Islandora relationship management functions. All commands are described here along with
 *  common use functions. Specific custom functions will need to be included in an .inc file and referenced here.
 *  Put this file wherever you put your Drush commands.
 *  Example: sites/all/drush/
 */

require 'includes/islandora_hasmodel_swap.inc';

/**
 * Implementation of hook_drush_command().
 *
 * @return array
 *   An associative array describing your command(s).
 */
function islandora_relationship_management_drush_command() {

  $items = array();

  $items['islandora-relationship-management'] = array(
    'description' => '',
    'arguments' => array(
      'file' => '',
    ),
    'examples' => array(
      'drush -u 1 relsmanage',
    ),
    'aliases' => array('relsmanage'),
    // We can pass in users id on the command line using drush -u.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  $items['hasmodel-swap'] = array(
    'description' => 'Drush script which accepts a list of PIDs in a text file to update the RELS-EXT hasModel relationship.',
    'arguments' => array(
      'pid_listfile' => 'The path relative to the Drupal root and file name + .txt extension of a list of PIDs to be updated.',
      'old_hasmodel_pid' => 'Namespace prefix and name (PID) of the hasModel relationship to find and update.',
      'new_hasmodel_pid' => 'Namespace prefix and name (PID) of the hasModel relationship to update to.',
      'interactive' => 'If true user will be asked to confirm operation on each object.',
    ),
    // Testing passing of command prompts.
    'argument_prompt' => array(
      'pid_list_file' => '',
      'old_hasmodel_pid' => '',
    ),
    'examples' => array(
      'drush -u 1 hasswap sites/all/drush/update_pids.txt islandora:newspaperCollectionCModel islandora:newspaperCModel true',
      //'drush -u 1 hasswap sites/all/drush/update_pids.txt add islandora:newspaperCModel true',
      //'drush -u 1 hasswap sites/all/drush/update_pids.txt islandora:newspaperCollectionCModel delete true',
    ),
    'aliases' => array('hasswap'),
    // We can pass in users id on the command line using drush -u.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

function drush_islandora_relationship_management() {

  drush_print("\n\n+-------------------------+\n  Relationship Management  \n+-------------------------+");


  $relsmanage_menu[1] = "Add hasModel relationship to RELS-EXT datastream";
  $relsmanage_menu[2] = "Delete hasModel relationship from RELS-EXT datastream";
  //$relsmanage_menu[3] = "Options";

  drush_print("\n");
}

function drush_islandora_hasmodel_add ($object_pid, $new_hasmodel_pid) {

  try {
    $object->relationships->add('info:fedora/fedora-system:def/model#', 'hasModel', $new_hasmodel_pid);
    drush_print("\n>>> RELATIONSHIP ADDED >>> $object->id");
    $GLOBALS['hasmodel_update_count']++;
  }
  catch (Exception $e) {
    drush_print ("Addition failed for $object->id \n\n$e->getMessage()");
  }
}

function drush_islandora_hasmodel_delete ($object_pid, $old_hasmodel_pid) {

  try {
    $object->relationships->remove('info:fedora/fedora-system:def/model#', 'hasModel', $old_hasmodel_pid);
    drush_print("\n>>> RELATIONSHIP DELETED >>> $object->id");
    $GLOBALS['hasmodel_update_count']++;
  }
  catch (Exception $e) {
    drush_print ("Deletion failed for $object->id \n\n$e->getMessage()");
  }
}