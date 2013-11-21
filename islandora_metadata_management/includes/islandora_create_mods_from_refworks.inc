<?php

/**
 * This is the Drush command specified in the array create by the drush entry point.
 *
 * This function checks to make sure parameters are supplied and if everything is ok
 *
 * @param string $pid_list_file
 *  path to a text file ...
 * @param string $xslt_file
 * @param boolean $interactive
 *
 */
function drush_islandora_metadata_management_create_mods_from_refworks($pid_list_file, $xslt_file, $interactive) {

  if (isset($pid_list_file)) {
    drush_print(" using list file " . $pid_list_file);
  }
  else {
    drush_print(" no list of pids file found");
    return;
  }
  if (isset($xslt_file)) {
    drush_print(" using xslt file " . $xslt_file);
  }
  else {
    drush_print("no xslt file found");
    return;
  }

  islandora_create_mods_from_refworks($pid_list_file, $xslt_file, $interactive);
}

/**
 * Iterates through all the objects in the PID list file and updates the mods stream
 *
 * @param string $pid_list_file
 * @param string $xslt_file
 * @param boolean $interactive
 *
 */
function islandora_create_mods_from_refworks($pid_list_file, $xslt_file, $interactive = FALSE) {

  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  ini_set("auto_detect_line_endings", "1");
  $xsl = file_get_contents($xslt_file);
  $pids = file($pid_list_file);
  $xslt = new XSLTProcessor();
  $xslt->importStylesheet(new SimpleXMLElement($xsl));

  //dpr($xsl);

  if (!isset($pids)) {
    drush_die('Could not read list of pids from file');
    return;
  }
  if (!isset($xsl)) {
    drush_die('Could not load contents of xslt file');
    return;
  }


  $num = count($pids);
  if (!drush_confirm(dt('are you sure you want to update all @num objects?', array('@num' => $num)))) {
    drush_die('Aborting.');
  }
  $count = 0;

  if ($interactive == 'TRUE') {
    foreach ($pids as $object) {
      $object = trim($object);
      if (drush_confirm(dt('update %dsid in this @pid ?', array('@pid' => $object, '%dsid' => 'MODS')))) {
        islandora_update($object, $xslt, 'MODS');
        drush_print("processed $object # " . $count++);
      }
    }
  }
  else {
    foreach ($pids as $object) {
      $object = trim($object);
      islandora_update($object, $xslt, 'MODS');
      //drush_print("processed $object # " . $count++);
      if($count > 1000){
        drush_die('Reached limit of 1000 stopping');
      }
    }
  }
}

/**
 * update all the objects
 *
 * @param string $pid
 * @param string $xslt
 * @param string $dsid
 *
 */
function islandora_update($pid, $xslt, $dsid) {

  module_load_include('inc', 'fedora_repository', 'api/fedora_item');


  /*
  $item = new Fedora_Item($pid);
  if (!isset($item)) {
    islandora_update_write_log(dt('error creating object %pid', array('%pid' => $pid)), 'islandora_update_from_xsl.log', 'error');
    return;
  }
  */

  $object = islandora_object_load($pid);

  $refworks = $object['REFWORKS']->content;
  if(!$refworks)
      drush_print("Refworks datastream not found on object $pid.");
  //drush_print($refworks->content);

  //dpr($xslt);
  /*
  $metadata = $item->get_datastream_dissemination('refworks');
  */
  $sxml = simplexml_load_string($refworks);

  //dpr($sxml);


  if (!$sxml) {
    //islandora_update_write_log(dt('error processing %pid', array('%pid' => $pid)), 'islandora_update_from_xsl.log', 'error');
    //$item->forget();
    return;
  }


  $content = $xslt->transformToXml($sxml);

  //dpr($content);

  // Create DS or grab it.
  if (!isset($object["MODS"])) {
    $mods = $object->constructDatastream('MODS', 'M');
  }
  else {
    $mods = $object["MODS"];
  }
  $mods->label = 'MODS Record';
  $mods->mimeType = 'text/xml';




// Don't copy the file.
  //$mods->setContentFromFile($content, FALSE);
  $mods->content = $content;
// May not need to be called if the DS already existed
  if($object->ingestDatastream($mods))
    drush_print("Successfully updated MODS datastream.");
  else
    drush_print("Failed updating MODS datastream.");


  /*
  //need to parse and add the doi easier in php then xslt
  $content = islandora_update_from_csv_process_xml($content, $metadata);
  //var_dump($content);
  if (isset($content)) {
    $value = $item->modify_datastream($content, $dsid, $dsid, 'text/xml', $force = FALSE, $logMessage = 'Modified by Islandora API using islandora_update_book_mods drush script', TRUE);
  }
  if (isset($value)) {
    islandora_update_write_log(dt('successfully updated %dsid for %pid', array('%dsid' => $dsid, '%pid' => $pid)), 'islandora_update_from_xsl.log');
  }
  else {
    islandora_update_write_log(dt('error updating %dsid for %pid', array('%dsid' => $dsid, '%pid' => $pid)), 'islandora_update_from_xsl.log', 'error');
  }
  time_nanosleep(0, 200000000);

  $item->forget();

  */
}

