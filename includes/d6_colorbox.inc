<?php

/**
 * @file
 *   Code to replace image tags using lightbox with colorbox in the node body.
 *
 * Problem being solved: D6 site used lightbox for image uploads. The image
 * tags are not separate fields but are inline in the body.
 * These need to be decorated to work with colorbox since lightbox has no D8
 * version.
 */
use Drupal\migrate\Row;
use Drupal\migrate\Plugin\MigrateSourceInterface;
use Drupal\migrate\Plugin\MigrationInterface;

function capetrials_custom_migration_colorbox(Row $row, MigrateSourceInterface $source, MigrationInterface $migration) {

  // List fields that use long text fields with text formats.
  $text_fields = array(
    'body',
  );

  // Update nodes.
  $len = strlen('node') + strlen(CUSTOM_MIGRATION_PREFIX);
  drush_print_r($migration->id());
  if (substr($migration->id(), 0, $len) == CUSTOM_MIGRATION_PREFIX . 'node') {
    foreach ($text_fields as $text_field) {
      drush_print_r($row);
      $value = $row->getSourceProperty($text_field);
      if (!empty($value) && preg_match('/img/', $value)) {
        drush_print_r("*****>>>>> regex value=\n" . $value . "\n");
        // $value[0]['format'] = $format_map[$format];
        // $row->setSourceProperty($text_field, $value);
      }
    }
  }
}

// vim:set et sw=2 ft=php ai:
