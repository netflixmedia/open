if (typeof (OpenSearchServer) == "undefined")
	OpenSearchServer = {};

this.OpenSearchServer.target = '';
this.OpenSearchServer.selectedAutocomplete = 0;
this.OpenSearchServer.autocompleteSize = 0;

OpenSearchServer.setAutocomplete = function(value) {
	if (OpenSearchServer.target == 'top') {
		var ac = document.getElementById('autocomplete_top');
	} else {
		var ac = document.getElementById('autocomplete');
	}
	ac.innerHTML = value;
	return ac;
};

OpenSearchServer.getselectedautocompletediv = function(n) {
	return document.getElementById('autocompleteitem' + n);
};

OpenSearchServer.getTarget = function(event) {
	if (event.target.id == 'edit-search-block-form-1') {
		OpenSearchServer.target = 'top';
	} else {
		OpenSearchServer.target = '';
	}
};

OpenSearchServer.autosuggest = function(event, clean_url) {
	OpenSearchServer.getTarget(event);
	var keynum;
	if (window.event) {
		keynum = event.keyCode;
	} else if (event.which) {
		keynum = event.which;
	}
	if (keynum == 38 || keynum == 40) {
		if (OpenSearchServer.selectedAutocomplete > 0) {
			OpenSearchServer
					.autocompleteLinkOut(OpenSearchServer.selectedAutocomplete);
		}
		if (keynum == 38) {
			if (OpenSearchServer.selectedAutocomplete > 0) {
				OpenSearchServer.selectedAutocomplete--;
			}
		} else if (keynum == 40) {
			if (OpenSearchServer.selectedAutocomplete < OpenSearchServer.autocompleteSize) {
				OpenSearchServer.selectedAutocomplete++;
			}
		}
		if (OpenSearchServer.selectedAutocomplete > 0) {
			var dv = OpenSearchServer
					.getselectedautocompletediv(OpenSearchServer.selectedAutocomplete);
			OpenSearchServer
					.autocompleteLinkOver(OpenSearchServer.selectedAutocomplete);
			OpenSearchServer.setKeywords(dv.innerHTML);
		}
		return false;
	}

	if (OpenSearchServer.target == 'top') {
		var str = escape(document.getElementById('edit-search-block-form-1').value);
	} else {
		var str = escape(document.getElementById('edit-q').value);
	}

	if (typeof (clen_url) == "undefined" || clean_url == 0)
		var url = '/?q=opensearchserver/autocomplete/&query=' + str;
	else
		var url = '/opensearchserver/autocomplete/?query=' + str;
	if (str.length == 0) {
		OpenSearchServer.setAutocomplete('');
		return;
	} else {
		$('#edit-q').addClass('load');
		$.post(url, function(data) {
			if (data.length > 0) {
				OpenSearchServer.handleAutocomplete(data);
			}
		});
	}
	return true;
};

OpenSearchServer.handleAutocomplete = function(data) {
	var ac = OpenSearchServer.setAutocomplete('');
	var resp = data;
	if (resp == null) {
		return;
	}
	if (resp.length == 0) {
		return;
	}
	var str = resp.split("\n");
	var content = '<div id="autocompletelist">';
	for (i = 0; i < str.length; i++) {
		var j = i + 1;
		content += '<div id="autocompleteitem' + j + '" ';
		content += 'onmouseover="javascript:OpenSearchServer.autocompleteLinkOver('
				+ j + ');" ';
		content += 'onmouseout="javascript:OpenSearchServer.autocompleteLinkOut('
				+ j + ');" ';
		content += 'onclick="javascript:OpenSearchServer.setsetKeywords_onClick(this.innerHTML);" ';
		content += 'class="autocomplete_link">' + str[i] + '</div>';
	}
	content += '</div>';
	ac.innerHTML = content;
	OpenSearchServer.selectedAutocomplete = 0;
	OpenSearchServer.autocompleteSize = str.length;
};

OpenSearchServer.autocompleteLinkOver = function(n) {
	if (OpenSearchServer.selectedAutocomplete > 0) {
		OpenSearchServer
				.autocompleteLinkOut(OpenSearchServer.selectedAutocomplete);
	}
	var dv = OpenSearchServer.getselectedautocompletediv(n);
	dv.className = 'autocomplete_link_over';
	OpenSearchServer.selectedAutocomplete = n;
};

OpenSearchServer.autocompleteLinkOut = function(n) {
	var dv = OpenSearchServer.getselectedautocompletediv(n);
	dv.className = 'autocomplete_link';
};

OpenSearchServer.setsetKeywords_onClick = function(value) {
	if (OpenSearchServer.target == 'top') {
		var dv = document.getElementById('edit-search-block-form-1');
	} else {
		var dv = document.getElementById('edit-q');
	}
	if (dv != null) {
		dv.value = value;
		dv.focus();
		OpenSearchServer.setAutocomplete('');
		document.forms['opensearchserver-page-form'].submit()
		return true;
	}
};

OpenSearchServer.setKeywords = function(value) {
	if (OpenSearchServer.target == 'top') {
		var dv = document.getElementById('edit-search-block-form-1');
	} else {
		var dv = document.getElementById('edit-q');
	}
	if (dv != null) {
		dv.value = value;
		dv.focus();
	}
};