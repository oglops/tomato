<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN'>
<!--
	Tomato GUI
	Copyright (C) 2006-2010 Jonathan Zarate
	http://www.polarcloud.com/tomato/

	For use with Tomato Firmware only.
	No part of this file may be used without permission.
-->
<html>
<head>
<meta http-equiv='content-type' content='text/html;charset=utf-8'>
<meta name='robots' content='noindex,nofollow'>
<title>[<% ident(); %>] Admin: AdBlock</title>
<link rel='stylesheet' type='text/css' href='tomato.css'>
<link rel='stylesheet' type='text/css' href='color.css'>
<script type='text/javascript' src='tomato.js'></script>

<!-- / / / -->

<script type='text/javascript' src='debug.js'></script>

<script type='text/javascript'>

//	<% nvram("adblock_enable"); %>

fmtwait = (nvram.t_fix1 == 'RT-N16' ? 120 : 60);

function save()
{
	var fom = E('_fom');
        fom.adblock_enable.value = E('_f_adblock_enable').checked ? 1 : 0;
        form.submit(fom, 1);
}

function submit_complete()
{
	reloadPage();
}
</script>

</head>
<body>
<form id='_fom' method='post' action='tomato.cgi'>
<table id='container' cellspacing=0>
<tr><td colspan=2 id='header'>
	<div class='title'>Tomato</div>
	<div class='version'>Version <% version(); %></div>
</td></tr>
<tr id='body'><td id='navi'><script type='text/javascript'>navi()</script></td>
<td id='content'>
<div id='ident'><% ident(); %></div>

<!-- / / / -->

<input type='hidden' name='_nextpage' value='admin-adblock.asp'>
<input type='hidden' name='_service' value='adblock-restart'>

<input type='hidden' name='adblock_enable'>

<div class='section-title'>AdBlock</div>
<div class='section'>
<script type='text/javascript'>

adblock_enable = (nvram.adblock_enable == 1);
createFieldTable('', [
	{ title: 'Enable', name: 'f_adblock_enable', type: 'checkbox', value: adblock_enable },
]);
</script>
<br><br><b>ATTENTION</b><br><br>
Make sure that your admin interface is NOT running on port 80 (go to Administration - Admin Access and change<br>
your HTTP port to something != 80 [or use HTTPS instead]) before enabling AdBlock - otherwise you will not be<br>
able to login to your router anymore!<br><br>
Hostfiles for blocked domains will be downloaded to /tmp, so make sure that your router has ~5 MB free RAM!<br><br><br>
AdBlock is based on nullserv (http://github.com/flexiondotorg/nullserv)
</div>

<script type='text/javascript'>show_notice1('<% notice("adblock"); %>');</script>

<!-- / / / -->

</td></tr>
<tr><td id='footer' colspan=2>
	<span id='footer-msg'></span>
	<input type='button' value='Save' id='save-button' onclick='save()'>
	<input type='button' value='Cancel' id='cancel-button' onclick='javascript:reloadPage();'>
</td></tr>
</table>
</form>
</body>
</html>
