// All data handling is done by the C++ backend.
// This file only does small page behaviour: tabs, edit dialog, delete confirm.

const $ = (sel) => document.querySelector(sel);

// ---------- tabs ----------
function showTab(name) {
  document.querySelectorAll('.tab').forEach((b) => b.classList.toggle('active', b.dataset.tab === name));
  document.querySelectorAll('.panel').forEach((p) => p.classList.toggle('active', p.id === name));
}

document.querySelectorAll('.tab').forEach((btn) => {
  btn.addEventListener('click', () => showTab(btn.dataset.tab));
});

// the C++ server tells us which tab to open (after saving, searching, ...)
showTab(document.body.dataset.tab || 'owners');

// ---------- edit pet dialog ----------
document.querySelectorAll('.edit-btn').forEach((btn) => {
  btn.addEventListener('click', () => {
    $('#editPetId').value = btn.dataset.id;
    $('#editName').value = btn.dataset.name;
    $('#editSpecies').value = btn.dataset.species;
    $('#editBreed').value = btn.dataset.breed;
    $('#editGender').value = btn.dataset.gender;
    $('#editDob').value = btn.dataset.dob;
    $('#editColor').value = btn.dataset.color;
    $('#editDialog').showModal();
  });
});

$('#editCancel').addEventListener('click', () => $('#editDialog').close());

// ---------- delete confirmation ----------
document.querySelectorAll('.delete-form').forEach((form) => {
  form.addEventListener('submit', (e) => {
    if (!confirm('Are you sure you want to delete this pet and all its vaccinations?')) {
      e.preventDefault();
    }
  });
});

// ---------- hide message after a few seconds ----------
const msg = document.querySelector('.msg');
if (msg) {
  setTimeout(() => (msg.style.display = 'none'), 6000);
  // keep the URL clean so a refresh doesn't show the old message again
  history.replaceState(null, '', '/?tab=' + (document.body.dataset.tab || 'owners'));
}
