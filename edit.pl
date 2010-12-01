use Encode;
use URI;
use Web::Scraper;

my $menu = scraper {
	process "ul.food-set li", "foods[]" => scraper {
		process "img", title => '@alt';
		process "a", link => '@href';
	};
};
$menu->user_agent->env_proxy;

my $res = $menu->scrape(
	URI->new("http://www.mcdonalds.co.jp/menu/regular/index.html"));

for (@{$res->{foods}}) {
  set_text(encode('cp932', $_->{title}) . "\r\n");
  set_text("\t" . $_->{link} . "\r\n");
}

