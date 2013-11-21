<?php

/**
 * @file
 */

/**
 * This is the drush command specified in the array create by the drush entry point.
 *
 * @param string
 *  The path relative to the drupal root and file name + .txt extension of a list of PIDs to be updated.
 * @param string
 *  PID of the cmodel requiring an update.
 * @param string
 *  PID of the cmodel to update to.
 * @param boolean
 *  If true user will be asked to confirm operation on each object.
 */
function drush_islandora_relationship_management_hasmodel_swap($pid_list_file, $old_hasmodel_pid, $new_hasmodel_pid, $interactive = FALSE) {

  if (!$pid_list_file)
    drush_print("\nNeed list of PIDs! Filename cannot be empty.");

  elseif (!$old_hasmodel_pid OR !$new_hasmodel_pid)
     drush_print("\nOld and new hasModel PIDs required for swap!");

  // Requirements met--run script body.
  if($pid_list_file AND $old_hasmodel_pid AND $new_hasmodel_pid) {

    // Show title.
    drush_print("\n***************************\n* Islandora HasModel Swap *\n***************************");

    // Read the given file and create array of PIDs to update.
    $update_pids = file($pid_list_file, FILE_IGNORE_NEW_LINES);

    if ($update_pids) {

      // Track updates found and fixed.
      $GLOBALS['hasmodel_match_count'] = 0;
      $GLOBALS['hasmodel_update_count'] = 0;

      if ($interactive = strtoupper($interactive) == 'TRUE') {
        drush_print("\n>>> INTERACTIVE MODE ENGAGED");
      }

      else {
        $interactive = FALSE;
        if (drush_confirm(dt('@newline>>> WARNING >>> Interactive mode not detected.@newline    You are about to swap each hasModel relationship matching @old with @new for each PID found in the provided file, continue?', array('@newline' => "\n", '@old' => $old_hasmodel_pid, '@new' => $new_hasmodel_pid)))) {
          drush_print("\n>>> AUTO MODE ENGAGED");
        }
      }

      foreach ($update_pids as $pid) {
        $object = islandora_object_load($pid);

        if ($object) {
          if ($interactive) {
            if ($mode == "DELETE") {
              if (drush_confirm(dt('@newlineSearch for relationship to delete on object @pid', array('@newline' => "\n", '@pid' => $pid)))) {
                // Search for cmodel fix and continue if match found.
                if (drush_cmodel_find_fixes($object, $old_hasmodel_pid)) {
                  if (drush_confirm(dt('@newlineDelete relationship @old for object @pid',
                    array('@newline' => "\n", '@pid' => $pid, '@old' => $old_hasmodel_pid)))) {
                    drush_islandora_hasmodel_delete ($object, $old_hasmodel_pid);
                  }
                  else {
                    drush_print("Skipping deletion of $pid");
                  }
                }
              }
              else {
                drush_print("\nSkipping search for $pid");
              }
            }
            elseif ($mode == "ADD") {

              if (drush_confirm(dt('@newlineAdd relationship @new for object @pid',
                   array('@newline' => "\n", '@pid' => $pid, '@new' => $new_hasmodel_pid)))) {
                drush_islandora_hasmodel_add ($object, $new_hasmodel_pid);
              }
              else {
                  drush_print("$new_hasmodel_pid relationship NOT added to $pid");
                }
            }
            else {
              if (drush_confirm(dt('@newlineSearch for required update on object @pid', array('@newline' => "\n", '@pid' => $pid)))) {
                // Search for cmodel fix and continue if match found.
                if (drush_cmodel_find_fixes($object, $old_hasmodel_pid)) {
                  if (drush_confirm(dt('@newlineUpdate relationship for object @pid from@newline@old to @new',
                    array('@newline' => "\n", '@pid' => $pid, '@old' => $old_hasmodel_pid, '@new' => $new_hasmodel_pid)))) {
                      drush_islandora_hasmodel_swap($object, $old_hasmodel_pid, $new_hasmodel_pid);
                  }
                  else {
                      drush_print("Skipping update on: $pid");
                  }
                }
              }
              else {
                  drush_print("\nSkipping search for fix on: $pid");
              }
            }
          }
          else {
            // Auto mode.
            if ($mode == "ADD") {
              drush_islandora_hasmodel_add($object, $new_hasmodel_pid);
            }
            elseif ($mode == "DELETE") {
              if (drush_cmodel_find_fixes($object, $old_hasmodel_pid)) {
                drush_islandora_hasmodel_delete($object, $old_hasmodel_pid);
              }
            }
            else {
              if (drush_cmodel_find_fixes($object, $old_hasmodel_pid)) {
                drush_islandora_hasmodel_swap($object, $old_hasmodel_pid, $new_hasmodel_pid);
              }
            }
          }
        }

        else {
          drush_print("\nObject \"$pid\" not found.");
        }
      }

      unset($object);
      if ($mode == "DELETE") {
        drush_print("\nHasModel Swamp complete.\n\n***************\n    Found: " . $GLOBALS['hasmodel_match_count'] . "\n  Deleted: " . $GLOBALS['hasmodel_update_count'] . "\n***************\n");
      }
      elseif ($mode == "ADD") {
        drush_print("\nHasModel Swap complete.\n\n***************\n    Added: " . $GLOBALS['hasmodel_update_count'] . "\n***************\n");
      }
      else {
        drush_print("\nHasModel Swap complete.\n\n*************\n  Found: " . $GLOBALS['hasmodel_match_count'] . "\n  Fixed: " . $GLOBALS['hasmodel_update_count'] . "\n*************\n");
      }
    }

    else {
      drush_print("\nError reading infile \"$pid_list_file\", HasModel Swap terminating.\n");
    }
  }

  else {
    drush_print("\nHasModel Swap terminating.\n");
  }
}


function drush_cmodel_find_fixes($object, $old_cmodel_pid) {

  $fix_found = FALSE;

  // @TODO verify $rels
  if ($rels = $object->relationships->get('info:fedora/fedora-system:def/model#', 'hasModel')) {

    $no_match = 0;

    foreach ($rels as $rel) {
      if (isset($rel['object']['value'])) {
        if ($rel['object']['value'] == $old_cmodel_pid) {
          drush_print("\n>>> MATCH FOUND >>> Relationship matching \"$old_cmodel_pid\" was found on object $object->id");
          $fix_found = TRUE;
          $GLOBALS['hasmodel_match_count']++;
        }
        else {
          $no_match++;
          //$fix_found ? TRUE : FALSE;
        }
      }
      else {
        drush_print("\nNo relationships found for object $object->id.");
      }
    }

    if (!$fix_found AND $no_match > 0) {
      $message = "\nRelationships exist, but none matching \"$old_cmodel_pid\" were found on object $object->id";
    }
    elseif ($fix_found AND $no_match > 0) {
       $message = ">>> Other relationship(s) not matching \"$old_cmodel_pid\" were also found.";
    }
    /*
    else {
      $message = "Error.";
    }
    */

    //drush_print("nomatch: " . $no_match);

    if ($message) {
      drush_print($message);
    }
  }
  else {
    drush_print("\n>>> WARNING >>> No valid relationships found on object $object->id");
  }

  return $fix_found;
}

function drush_islandora_hasmodel_swap($object, $old_cmodel_pid, $new_cmodel_pid) {

  try {
      $object->relationships->remove('info:fedora/fedora-system:def/model#', 'hasModel', $old_cmodel_pid);
      $object->relationships->add('info:fedora/fedora-system:def/model#', 'hasModel', $new_cmodel_pid);
      drush_print("\n>>> UPDATE SUCCESS >>> $object->id");
      $GLOBALS['hasmodel_update_count']++;
  }
  catch (Exception $e) {
    drush_print ("Update failed for: $object->id \n\n$e->getMessage()");
  }
}