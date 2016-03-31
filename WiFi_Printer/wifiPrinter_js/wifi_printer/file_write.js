
exports.writeFile = function (message, path){
	require('fs').writeFile(path, message, function(err){
		if(err != null){
			return console.log('Write file err!');
		}	
		//console.log('Saved!');
	});
}