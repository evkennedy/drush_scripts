<?php

/**
 * @file
 *  Parent script file for Islandora metadata management functions. All commands are described here along with
 *  common use functions. Specific custom functions will need to be included in an .inc file and referenced here.
 *  Put this file wherever you put your Drush commands.
 *  Example: sites/all/drush/
 */

require 'includes/islandora_create_mods_from_refworks.inc';

/**
 * Implementation of hook_drush_command().
 *
 * @return array
 *   An associative array describing your command(s).
 */
function islandora_metadata_management_drush_command() {

  $items = array();

  $items['islandora-metadata-management'] = array(
    'description' => '',
    'arguments' => array(
      'file' => '',
    ),
    'examples' => array(
      'drush -u 1 metamanage',
    ),
    'aliases' => array('metamanage'),
    // We can pass in users id on the command line using drush -u.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

/*
  $items['mods-from-xsl'] = array(
    'description' => 'Drush script which accepts a list of PIDs in a text file to update the RELS-EXT hasModel relationship.',
    'arguments' => array(
      'file' => 'The path relative to the Drupal root and file name + .txt extension of a list of PIDs to be updated.',
      'old_cmodel_pid' => 'Namespace prefix and name (PID) of the hasModel relationship to find and update.',
      'new_cmodel_pid' => 'Namespace prefix and name (PID) of the hasModel relationship to update to.',
      'interactive' => 'If true user will be asked to confirm operation on each object.',
    ),
    'examples' => array(
      'drush -u 1 metamanage sites/all/drush/update_pids.txt islandora:newspaperCollectionCModel islandora:newspaperCModel true',
    ),
    'aliases' => array('modsfromxsl'),
    // We can pass in users id on the command line using drush -u.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
*/

  $items['create-mods-from-refworks'] = array(
    'description' => 'Reads a list of PIDs from a text file and updates the MODS of that object using an XSLT that crosswalks refworks XML to MODS.',
    'arguments' => array(
      'pid_list_file' => 'The path to a plain text file that contains a list of PIDs (one per line).',
      'xslt_file' => 'The path to an XSLT that can transform refworks XML to MODS. We may have different XLSTs depending on genre.',
      //'datastream_id' => 'the ID of the datastream to modify (will usually be MODS)',
      'interactive' => 'if TRUE then you will be asked to confirm the update for each object.',
    ),
    'examples' => array(
      'drush -u 1 modsfromref sites/all/drush/update_pids.txt sites/all/drush/refworks_to_mods_book.xslt true',
    ),
    'aliases' => array('modsfromref'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

function drush_islandora_metadata_management() {

  drush_print("\n\n+-------------------------+\n  Metadata Management  \n+-------------------------+");

  $metamanage_menu[1] = "";
  $metamanage_menu[2] = "";

  drush_print("\n");
}