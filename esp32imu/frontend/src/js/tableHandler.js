// Table update

export function dataUpdate(data) {

	const table = document.getElementById('data-table');
	
	if (!table) {
        //console.warn('data-table not found');
        return;
    }
    
	table.innerHTML = `<tr><td>X</td><td>${data.x.toFixed(2)}</td></tr>
					 <tr><td>Y</td><td>${data.y.toFixed(2)}</td></tr>
					 <tr><td>Z</td><td>${data.z.toFixed(2)}</td></tr>`;
    //return ;
}



