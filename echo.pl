#!/usr/bin/env perl

while(1)
{
    my @input;
    while (<STDIN>)
    {
        push @input, $_;
    }
    my $joined_input = join('', @input);
    print($joined_input);
}
