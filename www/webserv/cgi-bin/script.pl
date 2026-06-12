#!/usr/bin/perl

print "Content-Type: text/plain\n\n";

my @messages = (
    "Today is a good day to write C++98.",
    "Every bug is just an undocumented feature.",
    "Remember to close your file descriptors.",
    "One more test can save one hour of debugging.",
    "CGI is older than some developers."
);

my $index = time() % scalar(@messages);

print "Fortune Cookie:\n";
print "$messages[$index]\n";
