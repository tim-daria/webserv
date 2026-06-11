#!/usr/bin/perl

print "Content-Type: text/plain\n\n";

print "=== CGI PERL TEST ===\n";
print "Method: $ENV{'REQUEST_METHOD'}\n";
print "Query: $ENV{'QUERY_STRING'}\n";

if ($ENV{'REQUEST_METHOD'} eq "POST") {
    read(STDIN, my $body, $ENV{'CONTENT_LENGTH'});
    print "\n--- BODY ---\n";
    print $body;
}
