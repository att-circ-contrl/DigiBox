#!/usr/bin/perl
# Attention Circuits Control Laboratory - wlDigiBox helper scripts
# Helper library for using the Digi-Box from Perl.
# Written by Christopher Thomas.
# Copyright (c) 2020 by Vanderbilt University. This work is licensed under
# the Creative Commons Attribution-ShareAlike 4.0 International License.


#
# Includes
#

use strict;
use warnings;



#
# Global Variables
#


# Cached configuration values.

my ($DBOX_tickrate);
$DBOX_tickrate = 10000;



#
# Functions
#


# This queries the DigiBox to get the RTC rate.
# This updates the cached value.
# Arg 0 is the communications handle.
# Returns the number of RTC ticks per second.

sub DBOX_QueryTickRate
{
  my ($handle);
  my ($rawtext, $matchtext);

  $handle = $_[0];

  if (!(defined $handle))
  {
    print "### [DBOX_QueryTickRate]  Bad arguments.\n";
  }
  else
  {
    ($rawtext, $matchtext) = NCLI_QueryApp_Seriously($handle,
      "LQY\n", 'Verbosity',
      "### [DBOX_QueryTickRate]  Couldn't query tick rate.\n",
      undef);

    # Just check the raw text.
    if ($rawtext =~ m/Ticks\/sec:\s+(\d+)/)
    {
      $DBOX_tickrate = $1;
    }
    else
    {
      print "### [DBOX_QueryTickRate]  Couldn't find tick rate in response.\n";
    }
  }


  # Return the cached value.
  return $DBOX_tickrate;
}



# This queries the cached RTC tick rate, saved by DBOX_QueryTickRate().
# No arguments.
# Returns the number of RTC ticks per second.

sub DBOX_GetCachedTickRate
{
  return $DBOX_tickrate;
}



# This tells the DigiBox to start capturing input data.
# Arg 0 is the communications handle.
# Arg 1 is the sampling rate to use, or undef to report input changes.
# No return value.

sub DBOX_StartCapture
{
  my ($handle, $rate);

  $handle = $_[0];
  $rate = $_[1];  # May be undef.

  if (!(defined $handle))
  {
    print "### [DBOX_StartCapture]  Bad arguments.\n";
  }
  else
  {
    if (defined $rate)
    {
      $rate = int($rate);

      NCLI_SendCommand($handle, "LRA $rate\n");
    }
    else
    {
      NCLI_SendCommand($handle, "LCH\n");
    }
  }
}



# This tells the digibox to stop capturing input data.
# Arg 0 is the communications handle.
# No return value.

sub DBOX_StopCapture
{
  my ($handle);

  $handle = $_[0];

  if (!(defined $handle))
  {
    print "### [DBOX_StopCapture]  Bad arguments.\n";
  }
  else
  {
    NCLI_SendCommand($handle, "LST\n");
  }
}



# This parses a digibox capture stream. Pointers to two arrays are returned,
# containing timestamps and data words.
# Arg 0 is a scalar containing text to parse.
# Returns (times, words).

sub DBOX_ParseLogicStreamAsWords
{
  my ($rawtext, $times_p, $words_p);
  my (@linedata, $thisline, $thistime, $thisword, $had_data);
  my ($firsttime, $prevtime, $tofs, $twrap, $tick_rate);

  $rawtext = $_[0];

  $times_p = [];
  $words_p = [];

  if (!(defined $rawtext))
  {
    print "### [DBOX_ParseLogicStreamAsWords]  Bad arguments.\n";
  }
  else
  {
    @linedata = split(/^/m, $rawtext);

    $tick_rate = DBOX_GetCachedTickRate();

    $tofs = 0;
    $prevtime = -1;
    $firsttime = undef;

    foreach $thisline (@linedata)
    {
      # See if this line looks like a sample.

      $had_data = 1;

      if ($thisline =~ m/^\s*\+(\w\w\w\w\w\w\w\w)(\w\w\w\w)/)
      {
        # Full timestamp (8 hex digits).
        $thistime = hex($1);
        $thisword = hex($2);
        $twrap = (1 << 32);  # FIXME - Requires 64-bit integers.
      }
      elsif ($thisline =~ m/^\s*\+(\w\w\w\w\w\w)(\w\w\w\w)/)
      {
        # Partial timestamp (6 hex digits).
        $thistime = hex($1);
        $thisword = hex($2);
        $twrap = (1 << 24);
      }
      elsif ($thisline =~ m/^\s*\+(\w\w\w\w)(\w\w\w\w)/)
      {
        # Partial timestamp (4 hex digits).
        $thistime = hex($1);
        $thisword = hex($2);
        $twrap = (1 << 16);
      }
      else
      {
        # Not a data line.
        $had_data = 0;
      }


      # Process the sample if we had one.

      if ($had_data)
      {
        # Infer the real time from the wrapped time.
        # NOTE - This will lose its place if the interval between samples is
        # longer than the wrap time!

        if (!(defined $firsttime))
        { $firsttime = $thistime; }

        $thistime -= $firsttime;

        if (($thistime + $tofs) < $prevtime)
        { $tofs += $twrap; }
        $thistime += $tofs;
        $prevtime = $thistime;

        # Record the sample.

        push @$times_p, $thistime / $tick_rate;
        push @$words_p, $thisword;
      }
    }
  }

  return ($times_p, $words_p);
}



# This parses a digibox capture stream, interpreting words as pairs of
# bytes. Pointers to three arrays are returned, containing timestamps in
# seconds, the MS byte stream, and the LS byte stream.
# Arg 0 is a scalar containing text to parse.
# Returns (times, ms bytes, ls bytes).

sub DBOX_ParseLogicStreamAsBytePairs
{
  my ($rawtext, $times_p, $ms_p, $ls_p);
  my ($words_p, $thisword);

  $rawtext = $_[0];

  $times_p = [];
  $ms_p = [];
  $ls_p = [];

  if (!(defined $rawtext))
  {
    print "### [DBOX_ParseLogicStreamAsBytePairs]  Bad arguments.\n";
  }
  else
  {
    ($times_p, $words_p) = DBOX_ParseLogicStreamAsWords($rawtext);

    foreach $thisword (@$words_p)
    {
      push @$ms_p, (($thisword >> 8) & 0xff);
      push @$ls_p, ($thisword & 0xff);
    }
  }

  return ($times_p, $ms_p, $ls_p);
}



#
# Main Program
#


# Return true.
1;



#
# This is the end of the file.
#
